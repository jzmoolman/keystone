#include "enclave.h"
#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_illegal_insn.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_timer.h>
#include <sbi/sbi_trap.h>

/* Print trap info and exit enclave */
static void sbi_trap_error(const char *msg, int rc,
                           ulong mcause, ulong mtval, ulong mtval2,
                           ulong mtinst, struct sbi_trap_regs *regs)
{
    u32 hartid = current_hartid();

    sbi_printf("%s: hart%d: %s (error %d)\n", __func__, hartid, msg, rc);
    sbi_printf("%s: hart%d: mcause=0x%" PRILX " mtval=0x%" PRILX "\n",
               __func__, hartid, mcause, mtval);
    if (misa_extension('H')) {
        sbi_printf("%s: hart%d: mtval2=0x%" PRILX " mtinst=0x%" PRILX "\n",
                   __func__, hartid, mtval2, mtinst);
    }
    sbi_printf("%s: hart%d: mepc=0x%" PRILX " mstatus=0x%" PRILX "\n",
               __func__, hartid, regs->mepc, regs->mstatus);
    /* Print general purpose registers */
    sbi_printf("%s: hart%d: ra=0x%" PRILX " sp=0x%" PRILX "\n",
               __func__, hartid, regs->ra, regs->sp);
    sbi_printf("%s: hart%d: gp=0x%" PRILX " tp=0x%" PRILX "\n",
               __func__, hartid, regs->gp, regs->tp);
    sbi_printf("%s: hart%d: t0=0x%" PRILX " t1=0x%" PRILX "\n",
               __func__, hartid, regs->t0, regs->t1);
    /* Add other registers as needed */

    /* Exit enclave on trap */
    sbi_sm_exit_enclave(regs, rc);
}

/* Keystone SM trap handler for OpenSBI 1.6 */
void sbi_trap_handler_keystone_enclave(struct sbi_trap_regs *regs)
{
    int rc = SBI_ENOTSUPP;
    const char *msg = "trap handler failed";
    ulong mcause = csr_read(CSR_MCAUSE);
    ulong mtval = csr_read(CSR_MTVAL), mtval2 = 0, mtinst = 0;
    struct sbi_trap_info trap;

    if (misa_extension('H')) {
        mtval2 = csr_read(CSR_MTVAL2);
        mtinst = csr_read(CSR_MTINST);
    }

    /* Handle interrupts */
    if (mcause & (1UL << (__riscv_xlen - 1))) {
        mcause &= ~(1UL << (__riscv_xlen - 1));
        switch (mcause) {
        case IRQ_M_TIMER:
        case IRQ_M_SOFT:
            regs->mepc -= 4;
            sbi_sm_stop_enclave(regs, STOP_TIMER_INTERRUPT);
            regs->a0 = SBI_ERR_SM_ENCLAVE_INTERRUPTED;
            regs->mepc += 4;
            return;
        default:
            msg = "unhandled external interrupt";
            goto trap_error;
        };
    }
    struct sbi_trap_context tcntx = {
      .regs = *regs,
      .trap = trap,
    };

    /* Handle exceptions */
    switch (mcause) {
    case CAUSE_ILLEGAL_INSTRUCTION:
      rc = sbi_illegal_insn_handler(&tcntx);
      msg = "illegal instruction handler failed";
      break;

    case CAUSE_MISALIGNED_LOAD:
    case CAUSE_MISALIGNED_STORE:
        /* Redirect misaligned accesses to OS or supervisor */
        //trap.epc = regs->mepc;
        trap.cause  = mcause;
        trap.tval   = mtval;
        trap.tval2  = mtval2;
        trap.tinst  = mtinst;
        rc = sbi_trap_redirect(regs, &trap);
        msg = "misaligned access redirected";
        break;

    case CAUSE_SUPERVISOR_ECALL:
    case CAUSE_MACHINE_ECALL:
        rc = sbi_ecall_handler(&tcntx);
        msg = "ecall handler failed";
        break;

    default:
        /* Redirect any other trap */
        //trap.epc    = regs->mepc;
        trap.cause  = mcause;
        trap.tval   = mtval;
        trap.tval2  = mtval2;
        trap.tinst  = mtinst;
        rc = sbi_trap_redirect(regs, &trap);
        break;
    };

trap_error:
    if (rc)
        sbi_trap_error(msg, rc, mcause, mtval, mtval2, mtinst, regs);
}

