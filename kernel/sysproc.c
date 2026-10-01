#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_va2pa(void)
{
  uint64 va;
  pte_t *pte;
  struct proc *p = myproc();

  argaddr(0, &va);                    // 0번째 인자를 주소로 읽는다
  if (va >= MAXVA)
    return 0;                         // 이대로 walk 에 주면 커널이 panic 한다

  // TODO ①  va 의 칸 번호 셋과 페이지 안 오프셋을 한 줄로 출력한다.
  //          printk("va %p : L2=%d L1=%d L0=%d off=0x%x\n", ...);
  //          - 칸 번호 셋 : PX(2, va) · PX(1, va) · PX(0, va)
  //          - 오프셋     : va 의 아래 12비트  (va & 0xFFF)
  printk("va %p : L2=%ld L1=%ld L0=%ld off=0x%xln\n",(void *)va, PX(2, va), PX(1, va), PX(0, va), (uint)(va & 0xFFF));
  // 가상 주소의 인덱스 및 오프셋 출력

  pte = walk(p->pagetable, va, 0);    // alloc = 0 : 찾기만 하고 만들지 않는다
  if (pte == 0 || (*pte & PTE_V) == 0)
    return 0;                         // 매핑이 없다
  if ((*pte & PTE_U) == 0)
    return 0;                         // 매핑은 있지만 사용자용이 아니다

  // TODO ②  va 의 물리 주소를 돌려준다. 두 조각을 더하면 된다.
  //          - 페이지 시작 주소 : PTE2PA(*pte)
  //          - 페이지 안 오프셋 : va 의 아래 12비트
  return PTE2PA(*pte) + (va & 0xFFF);
  // 최종  물리 주소 계산 및 반환
}
