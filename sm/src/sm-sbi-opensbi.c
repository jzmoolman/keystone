#include <sbi/sbi_trap.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_tlb.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_string.h>
#include <sbi/riscv_locks.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_scratch.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_ecall.h>
#include "sm-sbi-opensbi.h"
#include "pmp.h"
#include "sm-sbi.h"
#include "sm.h"
#include "cpu.h"





static int sbi_ecall_keystone_enclave_handler(unsigned long extid,
                                              unsigned long funcid,
                                              struct sbi_trap_regs *regs,
                                              struct sbi_ecall_return *out)
{
    //uintptr_t retval = SBI_ERR_SM_NOT_IMPLEMENTED;

    if (funcid <= FID_RANGE_DEPRECATED) {
        out->value = SBI_ERR_SM_DEPRECATED;
        return 0;
    } else if (funcid <= FID_RANGE_HOST) {
        if (cpu_is_enclave_context()) {
            out->value = SBI_ERR_SM_ENCLAVE_SBI_PROHIBITED;
            return 0;
        }
    } else if (funcid <= FID_RANGE_ENCLAVE) {
        if (!cpu_is_enclave_context()) {
            out->value = SBI_ERR_SM_ENCLAVE_SBI_PROHIBITED;
            return 0;
        }
    }

    switch (funcid) {
    case SBI_SM_CREATE_ENCLAVE:
        out->value = sbi_sm_create_enclave(&out->value, regs->a0);
        break;
    case SBI_SM_DESTROY_ENCLAVE:
        out->value = sbi_sm_destroy_enclave(regs->a0);
        break;
    case SBI_SM_RUN_ENCLAVE:
        out->value = sbi_sm_run_enclave((struct sbi_trap_regs *)regs, regs->a0);
        out->skip_regs_update = true; /* don’t overwrite regs after switching */
        return 0;
    /* ... other cases ... */
    default:
        out->value = SBI_ERR_SM_NOT_IMPLEMENTED;
        break;
    }

    return 0;
}





struct sbi_ecall_extension ecall_keystone_enclave = {
  .extid_start = SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE,
  .extid_end = SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE,
  .handle = sbi_ecall_keystone_enclave_handler,
};

//struct sbi_ecall_extension ecall_keystone_enclave = {
//  .extid_start = SBI_EXT_EXPERIMENTAL_KEYSTONE,
//  .extid_end = SBI_EXT_EXPERIMENTAL_KEYSTONE,
 //   .handle      = sbi_ecall_keystone_enclave_handler,
//};

