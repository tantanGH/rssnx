#include <stddef.h>
#include "utf8_cp932_data.c"

//
//  utf-8 to cp932
//
void convert_utf8_to_cp932(uint8_t* cp932_buffer, uint8_t* utf8_buffer, size_t utf8_bytes) {

  // CAUTION: utf8_buffer may be odd address and word access will fail on 68000 machines

  int16_t utf8_ofs = 0;
  int16_t cp932_ofs = 0;

  while (utf8_ofs < utf8_bytes) {
    uint8_t b0 = utf8_buffer[ utf8_ofs++ ];
    if (b0 <= 0x7f) {
      cp932_buffer[ cp932_ofs++ ] = b0;
    } else {
      uint16_t* code_map = NULL;
      int16_t code_map_len = 0;
      switch (b0) {
        case 0xc2: { code_map = utf8_to_cp932_c2; code_map_len = utf8_to_cp932_c2_len; break; }
        case 0xc3: { code_map = utf8_to_cp932_c3; code_map_len = utf8_to_cp932_c3_len; break; }
        case 0xce: { code_map = utf8_to_cp932_ce; code_map_len = utf8_to_cp932_ce_len; break; }
        case 0xcf: { code_map = utf8_to_cp932_cf; code_map_len = utf8_to_cp932_cf_len; break; }
        case 0xd0: { code_map = utf8_to_cp932_d0; code_map_len = utf8_to_cp932_d0_len; break; }
        case 0xd1: { code_map = utf8_to_cp932_d1; code_map_len = utf8_to_cp932_d1_len; break; }
        case 0xe2: { code_map = utf8_to_cp932_e2; code_map_len = utf8_to_cp932_e2_len; break; }
        case 0xe3: { code_map = utf8_to_cp932_e3; code_map_len = utf8_to_cp932_e3_len; break; }
        case 0xe4: { code_map = utf8_to_cp932_e4; code_map_len = utf8_to_cp932_e4_len; break; }
        case 0xe5: { code_map = utf8_to_cp932_e5; code_map_len = utf8_to_cp932_e5_len; break; }
        case 0xe6: { code_map = utf8_to_cp932_e6; code_map_len = utf8_to_cp932_e6_len; break; }
        case 0xe7: { code_map = utf8_to_cp932_e7; code_map_len = utf8_to_cp932_e7_len; break; }
        case 0xe8: { code_map = utf8_to_cp932_e8; code_map_len = utf8_to_cp932_e8_len; break; }
        case 0xe9: { code_map = utf8_to_cp932_e9; code_map_len = utf8_to_cp932_e9_len; break; }
        case 0xef: { code_map = utf8_to_cp932_ef; code_map_len = utf8_to_cp932_ef_len; break; }
      }
      if (code_map != NULL) {
        uint16_t key = 0;
        uint16_t value = 0;
        if (b0 <= 0xdf) {
          key = utf8_buffer[ utf8_ofs++ ];
        } else {
          key = utf8_buffer[ utf8_ofs ] * 256 + utf8_buffer[ utf8_ofs + 1 ];
          utf8_ofs += 2;
        }
        for (int16_t i = 0; i < code_map_len; i++) {
          if (code_map[ i * 2 ] == key) {
            value = code_map[ i * 2 + 1 ];
            break;
          }
        }
        if (value > 0) {
          cp932_buffer[ cp932_ofs++ ] = value / 256;
          cp932_buffer[ cp932_ofs++ ] = value & 0xff;
        }
      } else {
        cp932_buffer[ cp932_ofs++ ] = utf8_buffer[ utf8_ofs ];
      } 
    }
  }

  cp932_buffer[ cp932_ofs++ ] = '\0';

}