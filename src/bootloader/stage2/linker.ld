TARGET(binary)
INPUT("./build/bootloader/stage2/protected_setup.bin")

OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x20000;
    .text : {
      "./build/bootloader/stage2/protected_setup.bin"
    }
    .entry2              : { __entry2_start = .;      *(.entry2)   }
    .entry3              : { __entry3_start = .;      *(.entry3)   }
    .text               : { __text_start = .;       *(.text)    }
    .text.unlikely      : { *(.text.unlinkey)                   }
    .data               : { __data_start = .;       *(.data)    }
    .rodata             : { __rodata_start = .;     *(.rodata)  }
    .bss                : { __bss_start = .;        *(.bss)     }
    
    __end = .;
}
