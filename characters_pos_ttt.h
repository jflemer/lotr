int characters_pos[][10] = { {0x00a0, 0x0064, 0x00, 0xa0, 0x80, 0x00, 0xff, 0xff, 0x00, 0x00},  /*   0 */
{0x00b0, 0x005a, 0x00, 0xa1, 0x80, 0x00, 0xff, 0xff, 0x00, 0x00},       /*   1 */
{0x0798, 0x0852, 0x08, 0xa2, 0x80, 0x00, 0xff, 0xff, 0x02, 0x00},       /*   2 */
{0x07a4, 0x0852, 0x08, 0xa3, 0x80, 0x00, 0xff, 0xff, 0x02, 0x00},       /*   3 */
{0x0c1c, 0x00fc, 0x06, 0xa5, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*   4 */
{0x07e6, 0x05c1, 0x06, 0xa4, 0x80, 0x00, 0xff, 0xff, 0x01, 0x00},       /*   5 */
{0x07e6, 0x05c4, 0x06, 0xa7, 0x80, 0x00, 0xff, 0xff, 0x01, 0x00},       /*   6 */
{0x07e6, 0x05c6, 0x06, 0xa6, 0x80, 0x00, 0xff, 0xff, 0x01, 0x00},       /*   7 */
{0x0370, 0x0390, 0x10, 0xae, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*   8 */
{0x0000, 0x0000, 0x10, 0xc7, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*   9 */
{0x00dc, 0x00f4, 0x00, 0x12, 0x80, 0x00, 0x00, 0x00, 0xff, 0x00},       /*  10 */
{0x0185, 0x0097, 0x00, 0x82, 0x80, 0x01, 0x01, 0x01, 0xff, 0x00},       /*  11 */
{0x00a1, 0x0149, 0x00, 0x13, 0x80, 0x00, 0x01, 0x00, 0xff, 0x00},       /*  12 */
{0x076c, 0x0560, 0x10, 0x16, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  13 */
{0x0082, 0x0032, 0x01, 0xad, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  14 */
{0x0870, 0x0042, 0x01, 0x14, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  15 */
{0x0000, 0x0000, 0x10, 0xb2, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  16 */
{0x0000, 0x0000, 0x10, 0x11, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  17 */
{0x0000, 0x0000, 0x10, 0x10, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  18 */
{0x0000, 0x0000, 0x10, 0x0f, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  19 */
{0x0678, 0x0420, 0x10, 0x15, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  20 */
{0x0578, 0x079e, 0x02, 0x23, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  21 */
{0x06a4, 0x077a, 0x10, 0x7a, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  22 */
{0x01f0, 0x0070, 0x03, 0x79, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  23 */
{0x023c, 0x0520, 0x10, 0x6d, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  24 */
{0x0001, 0x0001, 0x05, 0x0b, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  25 */
{0x0000, 0x0000, 0x10, 0x1b, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  26 */
{0x0000, 0x0000, 0x10, 0x1c, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  27 */
{0x0000, 0x0000, 0x10, 0xaa, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  28 */
{0x0000, 0x0000, 0x10, 0x7f, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  29 */
{0x01d4, 0x0510, 0x06, 0x1e, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  30 */
{0x0228, 0x0578, 0x06, 0x8d, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /*  31 */
{0x023c, 0x064c, 0x06, 0x8d, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /*  32 */
{0x0000, 0x0000, 0x10, 0x98, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  33 */
{0x0380, 0x0198, 0x06, 0x1f, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  34 */
{0x0374, 0x019c, 0x06, 0x22, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  35 */
{0x0320, 0x0640, 0x06, 0x20, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  36 */
{0x042e, 0x068b, 0x06, 0x21, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  37 */
{0x01f4, 0x0758, 0x06, 0x24, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  38 */
{0x01f4, 0x0758, 0x10, 0xc2, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  39 */
{0x01d4, 0x0510, 0x10, 0xc8, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  40 */
{0x064c, 0x0834, 0x10, 0xc3, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  41 */
{0x0828, 0x00b4, 0x10, 0xc5, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  42 */
{0x0828, 0x00b4, 0x08, 0xbd, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  43 */
{0x0760, 0x05e8, 0x10, 0xa8, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  44 */
{0x010e, 0x07f4, 0x08, 0xa9, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  45 */
{0x0000, 0x0000, 0x10, 0xbe, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  46 */
{0x00e1, 0x004b, 0x08, 0xb5, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  47 */
{0x0001, 0x0001, 0x10, 0xac, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  48 */
{0x0050, 0x0580, 0x08, 0xb4, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  49 */
{0x05f8, 0x01d0, 0x08, 0x93, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /*  50 */
{0x05dc, 0x01d0, 0x10, 0xcc, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  51 */
{0x01c6, 0x01aa, 0x09, 0x64, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  52 */
{0x043a, 0x00a8, 0x09, 0x65, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  53 */
{0x0000, 0x0000, 0x10, 0x25, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  54 */
{0x010c, 0x00a6, 0x0a, 0xaf, 0x80, 0x00, 0x05, 0x00, 0xff, 0x00},       /*  55 */
{0x011c, 0x009e, 0x0a, 0xab, 0x80, 0x00, 0x05, 0x00, 0xff, 0x00},       /*  56 */
{0x0038, 0x012e, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x00, 0xff, 0x00},       /*  57 */
{0x01c0, 0x001e, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x00, 0xff, 0x00},       /*  58 */
{0x0120, 0x01c6, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x00, 0xff, 0x00},       /*  59 */
{0x0038, 0x01c6, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x00, 0xff, 0x00},       /*  60 */
{0x01cc, 0x010c, 0x0a, 0xb0, 0x80, 0x00, 0x05, 0x01, 0xff, 0x00},       /*  61 */
{0x004b, 0x003c, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x01, 0xff, 0x00},       /*  62 */
{0x01cc, 0x01aa, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x01, 0xff, 0x00},       /*  63 */
{0x01b8, 0x0126, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x02, 0xff, 0x00},       /*  64 */
{0x00e8, 0x01c2, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x02, 0xff, 0x00},       /*  65 */
{0x0010, 0x01be, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x03, 0xff, 0x00},       /*  66 */
{0x01c0, 0x0098, 0x0a, 0xcd, 0x80, 0x00, 0x05, 0x02, 0xff, 0x00},       /*  67 */
{0x013c, 0x0154, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x00, 0xff, 0x00},       /*  68 */
{0x01a4, 0x005c, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x01, 0xff, 0x00},       /*  69 */
{0x01b8, 0x005c, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x01, 0xff, 0x00},       /*  70 */
{0x01b0, 0x0050, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x01, 0xff, 0x00},       /*  71 */
{0x01b0, 0x0068, 0x0a, 0x7d, 0x80, 0x02, 0x05, 0x01, 0xff, 0x00},       /*  72 */
{0x010c, 0x01d0, 0x0a, 0x80, 0x80, 0x01, 0x05, 0x02, 0xff, 0x00},       /*  73 */
{0x0060, 0x0110, 0x10, 0x93, 0x80, 0x02, 0x05, 0x02, 0xff, 0x00},       /*  74 */
{0x0366, 0x08a2, 0x10, 0x77, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  75 */
{0x07f8, 0x0812, 0x0a, 0x1d, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  76 */
{0x0800, 0x0548, 0x0a, 0x7d, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /*  77 */
{0x0800, 0x0578, 0x0a, 0x7d, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /*  78 */
{0x081c, 0x045c, 0x0a, 0x7d, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /*  79 */
{0x01e8, 0x0368, 0x0a, 0xc6, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  80 */
{0x00d4, 0x00ac, 0x0a, 0x7d, 0x80, 0x02, 0x07, 0x00, 0xff, 0x00},       /*  81 */
{0x00d8, 0x00d4, 0x0a, 0xbb, 0x80, 0x00, 0x0b, 0x00, 0xff, 0x00},       /*  82 */
{0x00d2, 0x010e, 0x0a, 0x81, 0x80, 0x01, 0x0c, 0x00, 0xff, 0x00},       /*  83 */
{0x00a8, 0x009a, 0x0a, 0x7d, 0x80, 0x02, 0x0c, 0x00, 0xff, 0x00},       /*  84 */
{0x00d0, 0x00a8, 0x0a, 0xba, 0x80, 0x00, 0x0d, 0x00, 0xff, 0x00},       /*  85 */
{0x0098, 0x00a8, 0x0a, 0x7d, 0x80, 0x02, 0x0f, 0x00, 0xff, 0x00},       /*  86 */
{0x07d0, 0x0690, 0x0c, 0x17, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  87 */
{0x006e, 0x0120, 0x10, 0x18, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  88 */
{0x006e, 0x0120, 0x10, 0xcf, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  89 */
{0x006e, 0x0120, 0x10, 0xce, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  90 */
{0x0690, 0x03a2, 0x10, 0x19, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  91 */
{0x0690, 0x0276, 0x10, 0x73, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /*  92 */
{0x035a, 0x05dc, 0x0c, 0x1a, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  93 */
{0x071b, 0x06a4, 0x10, 0x26, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  94 */
{0x071b, 0x06a4, 0x10, 0x27, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  95 */
{0x071b, 0x06a4, 0x10, 0x28, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  96 */
{0x006e, 0x0120, 0x10, 0xeb, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /*  97 */
{0x00fc, 0x007e, 0x0c, 0x7d, 0x80, 0x02, 0x10, 0x00, 0xff, 0x00},       /*  98 */
{0x00ec, 0x010a, 0x0c, 0x87, 0x80, 0x01, 0x10, 0x00, 0xff, 0x00},       /*  99 */
{0x0084, 0x0098, 0x0c, 0x7d, 0x80, 0x02, 0x10, 0x00, 0xff, 0x00},       /* 100 */
{0x0094, 0x007c, 0x0c, 0x7d, 0x80, 0x02, 0x10, 0x00, 0xff, 0x00},       /* 101 */
{0x00a4, 0x0068, 0x0c, 0x7d, 0x80, 0x02, 0x10, 0x00, 0xff, 0x00},       /* 102 */
{0x0078, 0x00fc, 0x0c, 0x7d, 0x80, 0x02, 0x10, 0x00, 0xff, 0x00},       /* 103 */
{0x0078, 0x0118, 0x0c, 0x7d, 0x80, 0x02, 0x10, 0x00, 0xff, 0x00},       /* 104 */
{0x009c, 0x010c, 0x0c, 0x7d, 0x80, 0x02, 0x10, 0x00, 0xff, 0x00},       /* 105 */
{0x0224, 0x0188, 0x10, 0xc4, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 106 */
{0x0462, 0x003a, 0x10, 0x72, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 107 */
{0x0410, 0x00a0, 0x10, 0x72, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 108 */
{0x03b0, 0x0168, 0x0d, 0x7d, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 109 */
{0x03a4, 0x0036, 0x0d, 0x72, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 110 */
{0x044c, 0x00d8, 0x0d, 0x72, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 111 */
{0x0000, 0x0000, 0x10, 0xb9, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 112 */
{0x00f8, 0x019c, 0x0d, 0xb6, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 113 */
{0x008a, 0x00f6, 0x10, 0xb7, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 114 */
{0x035e, 0x0058, 0x10, 0xc9, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 115 */
{0x00e2, 0x014c, 0x0e, 0x8d, 0x80, 0x01, 0x11, 0x01, 0xff, 0x00},       /* 116 */
{0x0001, 0x0001, 0x0e, 0x09, 0x80, 0x00, 0x11, 0x00, 0xff, 0x00},       /* 117 */
{0x0100, 0x00d8, 0x0e, 0xd0, 0x80, 0x00, 0x16, 0x00, 0xff, 0x00},       /* 118 */
{0x00c0, 0x00a4, 0x0e, 0x81, 0x80, 0x01, 0x19, 0x00, 0xff, 0x00},       /* 119 */
{0x0087, 0x00aa, 0x0e, 0x7e, 0x80, 0x01, 0x12, 0x00, 0xff, 0x00},       /* 120 */
{0x0096, 0x00dc, 0x0e, 0x8a, 0x80, 0x01, 0x14, 0x00, 0xff, 0x00},       /* 121 */
{0x0104, 0x009a, 0x0e, 0x68, 0x80, 0x02, 0x18, 0x00, 0xff, 0x00},       /* 122 */
{0x00fd, 0x0094, 0x0e, 0x68, 0x80, 0x02, 0x18, 0x00, 0xff, 0x00},       /* 123 */
{0x010b, 0x00a0, 0x0e, 0x68, 0x80, 0x02, 0x18, 0x00, 0xff, 0x00},       /* 124 */
{0x00a0, 0x00aa, 0x0e, 0x68, 0x80, 0x02, 0x15, 0x00, 0xff, 0x00},       /* 125 */
{0x009c, 0x00a2, 0x0e, 0x68, 0x80, 0x02, 0x15, 0x00, 0xff, 0x00},       /* 126 */
{0x00a4, 0x00b0, 0x0e, 0x68, 0x80, 0x02, 0x15, 0x00, 0xff, 0x00},       /* 127 */
{0x00a0, 0x00b0, 0x0e, 0x68, 0x80, 0x02, 0x15, 0x00, 0xff, 0x00},       /* 128 */
{0x00f2, 0x015c, 0x0e, 0x8d, 0x80, 0x02, 0x11, 0x00, 0xff, 0x00},       /* 129 */
{0x00f2, 0x0178, 0x0e, 0x8d, 0x80, 0x02, 0x11, 0x00, 0xff, 0x00},       /* 130 */
{0x00f2, 0x0144, 0x0e, 0x8d, 0x80, 0x02, 0x11, 0x00, 0xff, 0x00},       /* 131 */
{0x00de, 0x0144, 0x0e, 0x8d, 0x80, 0x02, 0x11, 0x00, 0xff, 0x00},       /* 132 */
{0x00c2, 0x0108, 0x0e, 0x77, 0x80, 0x02, 0x11, 0x00, 0xff, 0x00},       /* 133 */
{0x00c6, 0x0120, 0x0e, 0x77, 0x80, 0x02, 0x11, 0x00, 0xff, 0x00},       /* 134 */
{0x00e2, 0x011c, 0x0e, 0x77, 0x80, 0x02, 0x11, 0x00, 0xff, 0x00},       /* 135 */
{0x011a, 0x013c, 0x0e, 0x04, 0x80, 0x01, 0x11, 0x02, 0xff, 0x00},       /* 136 */
{0x021c, 0x0174, 0x0e, 0x69, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 137 */
{0x0230, 0x0174, 0x0e, 0x69, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 138 */
{0x0224, 0x0188, 0x0e, 0xcb, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 139 */
{0x0099, 0x0105, 0x0f, 0x60, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 140 */
{0x0099, 0x0145, 0x0f, 0x61, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 141 */
{0x0099, 0x0185, 0x0f, 0x62, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 142 */
{0x0169, 0x0097, 0x0f, 0x66, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 143 */
{0x00ca, 0x0126, 0x0f, 0x67, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 144 */
{0x0126, 0x005a, 0x0f, 0x63, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 145 */
{0x012a, 0x0086, 0x0f, 0x05, 0x80, 0x02, 0xff, 0xff, 0xff, 0x00},       /* 146 */
{0x012a, 0x0087, 0x10, 0xd1, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 147 */
{0x012a, 0x0087, 0x10, 0xd2, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 148 */
{0x012a, 0x0087, 0x10, 0xd3, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 149 */
{0x012a, 0x0087, 0x10, 0xd4, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 150 */
{0x012a, 0x0087, 0x10, 0xd5, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 151 */
{0x012a, 0x0087, 0x10, 0xd6, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 152 */
{0x012a, 0x0087, 0x10, 0xd7, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 153 */
{0x012a, 0x0087, 0x10, 0xd7, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 154 */
{0x012a, 0x0087, 0x10, 0xd7, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 155 */
{0x012a, 0x0087, 0x10, 0xd7, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 156 */
{0x012a, 0x0087, 0x10, 0x29, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 157 */
{0x012a, 0x0087, 0x10, 0x2a, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 158 */
{0x012a, 0x0087, 0x10, 0x2b, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 159 */
{0x012a, 0x0087, 0x10, 0x2c, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 160 */
{0x012a, 0x0087, 0x10, 0x2d, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 161 */
{0x012a, 0x0087, 0x10, 0x2e, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 162 */
{0x012a, 0x0087, 0x10, 0x2f, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 163 */
{0x012a, 0x0087, 0x10, 0x30, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 164 */
{0x012a, 0x0087, 0x10, 0x31, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 165 */
{0x012a, 0x0087, 0x10, 0x32, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 166 */
{0x012a, 0x0087, 0x10, 0x33, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 167 */
{0x012a, 0x0087, 0x10, 0x34, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 168 */
{0x012a, 0x0087, 0x10, 0x35, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 169 */
{0x012a, 0x0087, 0x10, 0x36, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 170 */
{0x012a, 0x0087, 0x10, 0x37, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 171 */
{0x012a, 0x0087, 0x10, 0x38, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 172 */
{0x012a, 0x0087, 0x10, 0x39, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 173 */
{0x012a, 0x0087, 0x10, 0xd8, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 174 */
{0x012a, 0x0087, 0x10, 0xd9, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 175 */
{0x012a, 0x0087, 0x10, 0xda, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 176 */
{0x012a, 0x0087, 0x10, 0xdc, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 177 */
{0x012a, 0x0087, 0x10, 0xdd, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 178 */
{0x012a, 0x0087, 0x10, 0xde, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 179 */
{0x012a, 0x0087, 0x10, 0xdf, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 180 */
{0x012a, 0x0087, 0x10, 0xe1, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 181 */
{0x012a, 0x0087, 0x10, 0xe2, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 182 */
{0x012a, 0x0087, 0x10, 0xe3, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 183 */
{0x012a, 0x0087, 0x10, 0x3a, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 184 */
{0x012a, 0x0087, 0x10, 0x3b, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 185 */
{0x012a, 0x0087, 0x10, 0x3c, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 186 */
{0x012a, 0x0087, 0x10, 0x3d, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 187 */
{0x012a, 0x0087, 0x10, 0x3e, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 188 */
{0x012a, 0x0087, 0x10, 0x3f, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 189 */
{0x012a, 0x0087, 0x10, 0xe4, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 190 */
{0x012a, 0x0087, 0x10, 0xe5, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 191 */
{0x012a, 0x0087, 0x10, 0xe6, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 192 */
{0x012a, 0x0087, 0x10, 0xe7, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 193 */
{0x012a, 0x0087, 0x10, 0xe8, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 194 */
{0x012a, 0x0087, 0x10, 0x02, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 195 */
{0x012a, 0x0087, 0x10, 0x03, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 196 */
{0x012a, 0x0087, 0x10, 0xbf, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 197 */
{0x012a, 0x0087, 0x10, 0xc0, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 198 */
{0x012a, 0x0087, 0x10, 0xc1, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00},       /* 199 */
{0x03b4, 0x016c, 0x0d, 0x8b, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /* 200 */
{0x03a0, 0x0160, 0x0d, 0x40, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00},       /* 201 */
{0x0098, 0x0094, 0x0c, 0x8b, 0x80, 0x01, 0x10, 0x00, 0xff, 0x00},       /* 202 */
{0xffff, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};
