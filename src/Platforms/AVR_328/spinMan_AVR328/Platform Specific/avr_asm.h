//Some handy assembly routines. 

#ifndef _ASM_H_
   #define _ASM_H_
      
   /**********************************************
   ****** ARITHMETIC AND LOGIC INSTRUCTIONS ******
   **********************************************/
   
   // ### ADD ### Add Register src To Register dest (dest = dest + src)
   #define ASM_ADD(dest, src) asm volatile ("add %0, %1" : "=r" (dest) : "r" (src))
   
   // ### ADC ### Add Register src To Register dest With Carry (dest = dest + src + carry)
   #define ASM_ADC(dest, src) asm volatile ("adc %0, %1" : "=r" (dest) : "r" (src))
   
   // ### ADC_zero ### Add Carry To Register dest (dest = dest + carry), Special Form Of ADC
   #define ASM_ADCzero(dest) asm volatile ("adc %0, __zero_reg__" : "=r" (dest) :)
   
   // ### ADIW ### Add Constant To Word (word = word - const)
   #define ASM_ADIW(word, const) asm volatile ("adiw %0, %1" : "=w" (word) : "I" (const))
   
   // ### SUB ### Subtract Register src From Register dest (dest = dest - src)
   #define ASM_SUB(dest, src) asm volatile ("sub %0, %1" : "=r" (dest) : "r" (src))
   
   // ### SUBI ### Subtract Constant From Register (reg = reg - const)
   #define ASM_SUBI(reg, const) asm volatile ("subi %0, %1" : "=d" (reg) : "M" (const))
   
   // ### SBC ### Subtract Register src From Register dest With Carry (dest = dest - src - carry)
   #define ASM_SBC(dest, src) asm volatile ("sbc %0, %1" : "=r" (dest) : "r" (src))
   
   // ### SBC_zero ### Subtract Carry From Register dest (dest = dest - carry), Special Form Of SBC
   #define ASM_SBC_zero(dest) asm volatile ("sbc %0, __zero_reg__ " : "=r" (dest) :)
   
   // ### SBIW ### Subtract Constant From Register (reg = reg - const)
   #define ASM_SBIW(word, const) asm volatile ("sbiw %0, %1" : "=w" (word) : "I" (const))
   
   // ### AND ### Logical AND Of Two Registers (dest = dest & src)
   #define ASM_AND(dest, src) asm volatile ("and %0, %1" : "=r" (dest) : "r" (src))
   
   // ### ANDI ### Logical AND: Register and Constant (reg = reg & const)
   #define ASM_ANDI(reg, const) asm volatile ("andi %0, %1" : "=d" (reg) : "M" (const))
   
   // ### OR ### Logical OR Of Two Registers (dest = dest | src)
   #define ASM_OR(dest, src) asm volatile ("or %0, %1" : "=r" (dest) : "r" (src))
   
   // ### ORI ### Logical OR: Register and Constant (reg = reg | const)
   #define ASM_ORI(reg, const) asm volatile ("ori %0, %1" : "=d" (reg) : "M" (const))
   
   // ### EOR ### Logical exclusive OR Of Two Registers (dest = dest ^ src)
   #define ASM_EOR(dest, src) asm volatile ("eor %0, %1" : "=r" (dest) : "r" (src))
   
   // ### COM ### One’s Complement Of Register (reg = 0xFF - reg)
   #define ASM_COM(reg) asm volatile ("com %0" : "=r" (reg) : "0" (reg))
   
   // ### NEG ### Two’s Complement Of Register (reg = 0x00 - reg)
   #define ASM_NEG(reg) asm volatile ("neg %0" : "=r" (reg) : "0" (reg))
   
   // ### SBR ### Set Bit(s) in Register (reg = reg | mask)
   #define ASM_SBR(reg, mask) asm volatile ("sbr %0, %1" : "=d" (reg) : "M" (mask))
   
   // ### CBR ### Clear Bit(s) in Register (reg = reg & ~mask)
   #define ASM_CBR(reg, mask) asm volatile ("sbr %0, %1" : "=d" (reg) : "M" (mask))
   
   // ### INC ### Increment Register (reg = reg + 1)
   #define ASM_INC(reg) asm volatile ("inc %0" : "=r" (reg) : "0" (reg))
   
   // ### DEC ### Decrement Register (reg = reg - 1)
   #define ASM_DEC(reg) asm volatile ("dec %0" : "=r" (reg) : "0" (reg))
   
   // ### TST ### Test for Zero or Minus (reg = reg & reg)
   #define ASM_TST(reg) asm volatile ("tst %0" : "=r" (reg) : "0" (reg))
   
   // ### CLR ### Clear Register (reg = reg ^ reg)
   #define ASM_CLR(reg) asm volatile ("clr %0" : "=r" (reg) : "0" (reg))
   
   // ### SER ### Set Register (reg = 0xFF)
   #define ASM_SER(reg) asm volatile ("ser %0" : "=r" (reg) : "0" (reg))
   
   // ### MUL ### Multiply Two Unsigned Registers (r1:r0 = ureg1 * ureg2)
   #define ASM_MUL(ureg1, ureg2) asm volatile ("mul %0, %1" :: "r" (ureg1), "r" (ureg2))
   
   // ### MULS ### Multiply Two Signed Registers (r1:r2 = sreg1 * sreg2)
   #define ASM_MULS(sreg1, sreg2) asm volatile ("muls %0, %1" :: "d" (sreg1), "d" (sreg2))
   
   // ### MULSU ### Multiply Signed Register With Unsigned Register (r1:r2 = sreg * ureg)
   #define ASM_MULSU(sreg, ureg) asm volatile ("mulsu %0, %1" :: "a" (sreg), "a" (ureg))
   
   // ### FMUL ### Fractional Multiply Two Unsigned Registers (r1:r2 = (ureg1 * ureg2) << 1)
   #define ASM_FMUL(ureg1, ureg2) asm volatile ("fmul %0, %1" :: "a" (ureg1), "a" (ureg2))
   
   // ### FMULS ### Fractional Multiply Two Signed Registers (r1:r2 = (sreg1 * sreg2) << 1)
   #define ASM_FMULS(sreg1, sreg2) asm volatile ("fmuls %0, %1" :: "a" (sreg1), "a" (sreg2))
   
   // ### FMULSU ### Fractional Multiply Signed Register With Unsigned Register (r1:r2 = (sreg * ureg) << 1)
   #define ASM_FMULSU(sreg, ureg) asm volatile ("fmulsu %0, %1" :: "a" (sreg), "a" (ureg))
   
   
   
   /********************************
   ****** BRANCH INSTRUCTIONS ******
   *********************************/ 
   // ### ICALL ### Indirect Call To Z-Register (PC = Z)
   #define ASM_ICALL() asm volatile ("icall" :: )
   
   
   
   /***************************************
   ****** DATA TRANSFER INSTRUCTIONS ******
   ***************************************/
   
   // ### MOV ### Move Register src To Register dest (dest = src)
   #define ASM_MOV(dest, src) asm volatile ("mov %0, %1" : "=r" (dest) : "r" (src))
   
   // ### MOVW ### Move Register Word src To Register Word dest (dest = src)
   #define ASM_MOVW(dest, src) asm volatile ("movw %0, %1" : "=r" (dest) : "r" (src))
   
   // ### LDI ### Load Immediate Register With Constant (reg = const)
   #define ASM_LDI(reg, const) asm volatile ("ldi %0, %1" : "=d" (reg) : "M" (const))
   
   // ### LD ### Load Indirect By Using X, Y, Z Pointer (reg = *ptr)
   #define ASM_LD(reg, ptr) asm volatile ("ld %0, %a1" : "=r" (reg) : "e" (ptr))
   
   // ### LD ### 1. Decrement X, Y, Z, 2. Load Indirect By Using X Pointer (ptr = ptr - 1, reg = *ptr)
   #define ASM_decLD(reg, ptr) asm volatile ("ld %0, -%a1" : "=r" (reg) : "e" (ptr))
   
   // ### LD ### 1. Load Indirect By Using X, Y, Z Pointer, 2. Increment ptr (reg = *ptr, ptr = ptr + 1)
   #define ASM_LDinc(reg, ptr) asm volatile ("ld %0, %a1+" : "=r" (reg) : "e" (ptr))
   
   // ### STS ### Store Direct to SRAM
   // Storing Variable To SRAM:	static uint8_t var; uint8_t value = 123; STS (&var, value);
   // Storing Value To SFR:		uint8_t value = 5;	STS (_SFR_MEM_ADDR (SPMCR), value);
   #define ASM_STS(addr, reg) asm volatile ("sts %0, %1" : : "i" (addr),  "r" (reg))
   
   // ### LPM ### Load Program Memory (R0 = *Z)
   #define ASM_LPM() asm volatile ("lpm" ::)
   
   // ### LPM ### Load Program Memory (reg = *Z)
   #define ASM_LPM_Z(reg) asm volatile ("lpm %0, Z" : "=r" (reg))
   
   // ### LPM ### Load Program Memory and Post-Inc (reg = *Z, Z = Z + 1)
   #define ASM_LPM_Zinc(reg) asm volatile ("lpm %0, Z+" : "=r" (reg))
   
   // ### SPM ### Store Program Memory (*Z = R1:R0)
   #define ASM_SPM() asm volatile ("spm" ::)
   
   
   
   /******************************************
   ****** BIT AND BIT-TEST INSTRUCTIONS ******
   ******************************************/
   
   // ### SBI ### Set Pin Of A Port
   #define ASM_SBI(port, pin)   asm volatile ("sbi %0, %1" :: "I" (_SFR_IO_ADDR(port)), "I" (pin))
   
   // ### CBI ### Clear Pin Of A Port
   #define ASM_CBI(port, pin) asm volatile ("cbi %0, %1" :: "I" (_SFR_IO_ADDR(port)), "I" (pin))
   
   // ### LSL ### Logical Shift Left Of Register (reg = reg << 1)
   #define ASM_LSL(reg) asm volatile ("lsl %0" : "=r" (reg) : "0" (reg))
   
   // ### LSR ### Logical Shift Right Of Register (reg = reg >> 1)
   #define ASM_LSR(reg) asm volatile ("lsr %0" : "=r" (reg) : "0" (reg))
   
   // ### ROL ### Logical Rotate Left Of Register Through Carry
   #define ASM_ROL(reg) asm volatile ("rol %0" : "=r" (reg) : "0" (reg))
   
   // ### ROR ### Logical Rotate Right Of Register Through Carry
   #define ASM_ROR(reg) asm volatile ("ror %0" : "=r" (reg) : "0" (reg))
   
   // ### ASR ### Arithmetic Shift Right Of Register (Wie LSR Aber nur mit Bit 6 bis Bit 0)
   #define ASM_ASR(reg) asm volatile ("asr %0" : "=r" (reg) : "0" (reg))
   
   // ### SWAP ### Swap Nibbles Of Register (reg(0...3) = reg(4...7), reg(4...7) = reg(0...3))
   #define ASM_SWAP(reg) asm volatile ("swap %0" : "=r" (reg) : "0" (reg))
   
   // ### BSET ### Set Flag In Status Register (SREG(flagnumber) = 1)
   #define ASM_BSET(flagnumber) asm volatile ("bset %0" :: "I" (flagnumber))
   
   // ### BCLR ### Clear Flag In Status Register (SREG(flagnumber) = 0)
   #define ASM_BCLR(flagnumber) asm volatile ("bclr %0" :: "I" (flagnumber))
   
   // ### BST ### Bit Store From Register To T-Flag (T = reg(bitnumber))
   #define ASM_BST(reg, bitnumber) asm volatile ("bst %0, %1" ::  "r" (reg), "I" (bitnumber))
   
   // ### BLD ### Bit Load From T-Flag To Register (reg(bitnumber) = T)
   #define ASM_BLD(reg, bitnumber) asm volatile ("bld %0, %1" : "=r" (reg) : "I" (bitnumber))
   
   // ### SEC ### Set Carry Flag C in SREG
   #define ASM_SEC() asm volatile ("sec" :: )
   
   // ### CLC ### Clear Carry Flag C in SREG
   #define ASM_CLC() asm volatile ("clc" :: )
   
   // ### SEN ### Set Negative Flag N in SREG
   #define ASM_SEN() asm volatile ("sen" :: )
   
   // ### CLN ### Clear Negative Flag N in SREG
   #define ASM_CLN() asm volatile ("cln" :: )
   
   // ### SEZ ### Set Zero Flag Z in SREG
   #define ASM_SEZ() asm volatile ("sez" :: )
   
   // ### CLZ ### Clear Zero Flag Z in SREG
   #define ASM_CLZ() asm volatile ("clz" :: )
   
   // ### SEI ### Set Global Interrupt Enable I in SREG
   #define ASM_SEI() asm volatile ("sei" :: )
   
   // ### CLI ### Clear Global Interrupt Enable I in SREG
   #define ASM_CLI() asm volatile ("cli" :: )
   
   // ### SES ### Set Signed Test Flag in S SREG
   #define ASM_SES() asm volatile ("ses" :: )
   
   // ### CLS ### Clear Signed Test Flag S in SREG
   #define ASM_CLS() asm volatile ("cls" :: )
   
   // ### SEV ### Set Twos Complement Overflow Flag V in SREG
   #define ASM_SEV() asm volatile ("sev" :: )
   
   // ### CLV ### Clear Twos Complement Overflow Flag V in SREG
   #define ASM_CLV() asm volatile ("clv" :: )
   
   // ### SET ### Set Bit Copy Storage T in SREG
   #define ASM_SET() asm volatile ("set" :: )
   
   // ### CLT ### Clear Bit Copy Storage T in SREG
   #define ASM_CLT() asm volatile ("clt" :: )
   
   // ### SEH ### Set Half Carry Flag H in SREG
   #define ASM_SEH() asm volatile ("seh" :: )
   
   // ### CLH ### Clear Half Carry Flag H in SREG
   #define ASM_CLH() asm volatile ("clh" :: )
   
   
   
   /*************************************
   ****** MCU CONTROL INSTRUCTIONS ******
   *************************************/
   
   // ### BREAK ### Break
   #define ASM_BREAK() asm volatile ("break" :: )
   
   // ### NOP ### No Operation
   #define ASM_NOP() asm volatile ("nop" :: )
   
   // ### SLEEP ### Sleep
   #define ASM_SLEEP() asm volatile ("sleep" :: )
   
   // ### WDR ### Watchdog Reset
   #define ASM_WDR() asm volatile ("wdr" :: )
#endif