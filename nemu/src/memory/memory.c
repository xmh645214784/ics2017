#include "nemu.h"
#include "device/mmio.h"

paddr_t page_translate(vaddr_t vaddr);

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int i = is_mmio(addr);
  if (i == -1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  return mmio_read(addr, len, i) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int i = is_mmio(addr);
  if (i == -1)
    memcpy(guest_to_host(addr), &data, len);
  else
    mmio_write(addr, len, data, i);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if (((addr & 0xfff) + len) > 0x1000) {
    uint8_t temp[8];
    uint32_t temp_offset = addr & 3;
    
    paddr_t paddr = page_translate(addr);
    *(uint32_t *)(temp + temp_offset) = paddr_read(paddr, 4 - temp_offset);

    paddr = page_translate((addr & ~0xfff) + 0x1000);
    *(uint32_t *)(temp + 4) = paddr_read(paddr, len + temp_offset - 4);

    return (*(uint32_t *)(temp + temp_offset)) & (~0u >> ((4 - len) << 3));
  }
  paddr_t paddr = page_translate(addr);
  return paddr_read(paddr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if (((addr & 0xfff) + len) > 0x1000)
    assert(0);
  paddr_t paddr = page_translate(addr);
  paddr_write(paddr, len, data);
}

paddr_t page_translate(vaddr_t vaddr) {
  if (cpu.CR0.paging == 0)
    return vaddr;

  union {
    struct {
      uint32_t off   :12;
      uint32_t page  :10;
      uint32_t dir   :10;
    };
    uint32_t val;
  } addr;
  addr.val = vaddr;

  uint32_t pdb = cpu.CR3.page_directory_base;
  uint32_t tmp = addr.dir;
  uint32_t PDE_page_frame = paddr_read((pdb << 12) + (tmp << 2), 4);
  assert(PDE_page_frame & 0x1);

  tmp = addr.page;
  uint32_t PTE_page_frame = paddr_read((PDE_page_frame & 0xfffff000) + (tmp << 2), 4);
  assert(PTE_page_frame & 0x1);

  return (PTE_page_frame & 0xfffff000) + addr.off;
}