#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
//  xc headers
//
#include <doslib.h>
#include <process.h>

//
//  local headers
//
#include "cp932rsc.h"
#include "rssnx.h"
#include "utf8_cp932.h"
#include "yxml.h"

//
//  quick hack for elf2x68k link error
//
#ifdef __ELF2X68K__
#include <reent.h>
struct _reent _impure_data = {0};
struct _reent *_impure_ptr = _GLOBAL_REENT;
#endif

//
//  global buffers
//
static uint8_t g_fread_buffer[FREAD_BUF_SIZE];
static uint8_t g_stack_buffer[STACK_SIZE];
static uint8_t g_content_buffer[BUF_SIZE];
static uint8_t g_cp932_buffer[BUF_SIZE * 2];

//
//  convert utf-8 string to cp932
//
static void convert_utf8_to_cp932(uint8_t *cp932_buffer, uint8_t *utf8_buffer,
                                  size_t utf8_bytes) {

  // CAUTION: utf8_buffer may be odd address and word access will fail on 68000 machines

  int16_t utf8_ofs = 0;
  int16_t cp932_ofs = 0;

  while (utf8_ofs < utf8_bytes) {
    uint8_t b0 = utf8_buffer[utf8_ofs++];
    if (b0 <= 0x7f) {
      cp932_buffer[cp932_ofs++] = b0;
    } else {
      uint16_t *code_map = NULL;
      int16_t code_map_len = 0;
      switch (b0) {
      case 0xc2: {
        code_map = utf8_to_cp932_c2;
        code_map_len = utf8_to_cp932_c2_len;
        break;
      }
      case 0xc3: {
        code_map = utf8_to_cp932_c3;
        code_map_len = utf8_to_cp932_c3_len;
        break;
      }
      case 0xce: {
        code_map = utf8_to_cp932_ce;
        code_map_len = utf8_to_cp932_ce_len;
        break;
      }
      case 0xcf: {
        code_map = utf8_to_cp932_cf;
        code_map_len = utf8_to_cp932_cf_len;
        break;
      }
      case 0xd0: {
        code_map = utf8_to_cp932_d0;
        code_map_len = utf8_to_cp932_d0_len;
        break;
      }
      case 0xd1: {
        code_map = utf8_to_cp932_d1;
        code_map_len = utf8_to_cp932_d1_len;
        break;
      }
      case 0xe2: {
        code_map = utf8_to_cp932_e2;
        code_map_len = utf8_to_cp932_e2_len;
        break;
      }
      case 0xe3: {
        code_map = utf8_to_cp932_e3;
        code_map_len = utf8_to_cp932_e3_len;
        break;
      }
      case 0xe4: {
        code_map = utf8_to_cp932_e4;
        code_map_len = utf8_to_cp932_e4_len;
        break;
      }
      case 0xe5: {
        code_map = utf8_to_cp932_e5;
        code_map_len = utf8_to_cp932_e5_len;
        break;
      }
      case 0xe6: {
        code_map = utf8_to_cp932_e6;
        code_map_len = utf8_to_cp932_e6_len;
        break;
      }
      case 0xe7: {
        code_map = utf8_to_cp932_e7;
        code_map_len = utf8_to_cp932_e7_len;
        break;
      }
      case 0xe8: {
        code_map = utf8_to_cp932_e8;
        code_map_len = utf8_to_cp932_e8_len;
        break;
      }
      case 0xe9: {
        code_map = utf8_to_cp932_e9;
        code_map_len = utf8_to_cp932_e9_len;
        break;
      }
      case 0xef: {
        code_map = utf8_to_cp932_ef;
        code_map_len = utf8_to_cp932_ef_len;
        break;
      }
      }
      if (code_map != NULL) {
        uint16_t key = 0;
        uint16_t value = 0;
        if (b0 <= 0xdf) {
          key = utf8_buffer[utf8_ofs++];
        } else {
          key = utf8_buffer[utf8_ofs] * 256 + utf8_buffer[utf8_ofs + 1];
          utf8_ofs += 2;
        }
        for (int16_t i = 0; i < code_map_len; i++) {
          if (code_map[i * 2] == key) {
            value = code_map[i * 2 + 1];
            break;
          }
        }
        if (value > 0) {
          cp932_buffer[cp932_ofs++] = value / 256;
          cp932_buffer[cp932_ofs++] = value & 0xff;
        }
      } else {
        cp932_buffer[cp932_ofs++] = utf8_buffer[utf8_ofs];
      }
    }
  }

  cp932_buffer[cp932_ofs++] = '\0';
}

//
//  output plain text outside tags
//
static void print_plain_text(FILE *fo, const uint8_t *cp932_str) {
  int16_t in_tag = 0;
  for (const uint8_t *p = cp932_str; *p; p++) {
    if (*p == '<')
      in_tag = 1;
    else if (*p == '>')
      in_tag = 0;
    else if (!in_tag)
      fputc(*p, fo);
  }
}

//
//  output wrapped article title text
//
static void smart_wrap_print(FILE *fo, const uint8_t *str, int max_columns) {

  int16_t current_col = 0;
  const uint8_t *p = (const uint8_t *)str;

  while (*p != '\0') {

    int16_t char_len;
    int16_t char_col;

    // check sjis 2 bytes
    if ((*p >= 0x81 && *p <= 0x9f) || (*p >= 0xe0 && *p <= 0xfc)) {
      char_len = 2;
      char_col = 2; // zenkaku
    } else {
      char_len = 1;
      char_col = 1; // hankaku
    }

    // need newline?
    if (current_col + char_col > max_columns) {
      // fputc('\n', fo);
      fprintf(fo, "%c\n\n%%V%%W", 0x18); // 2 newlines are required to avoid overlapped display on DSHELL
      current_col = 0;
    }

    // output a character
    for (int i = 0; i < char_len; i++) {
      if (*p) {
        fputc(*p, fo);
        p++;
      }
    }
    current_col += char_col;
  }
  fputc('\n', fo);
}

//
//  parse and convert RSS XML to DSHELL format
//
static int32_t parse_rss(FILE *fp, FILE *fo) {

  int32_t rc = 0;

  yxml_t x;
  yxml_init(&x, g_stack_buffer, STACK_SIZE);

  int16_t buf_idx = 0;
  int16_t in_target_tag = TAG_NONE;
  int16_t in_item = 0;
  int16_t in_title = 0;
  uint8_t date_cache[64];

  size_t fread_len;
  while ((fread_len = fread(g_fread_buffer, 1, sizeof(g_fread_buffer), fp)) >
         0) {
    for (size_t i = 0; i < fread_len; i++) {
      yxml_ret_t r = yxml_parse(&x, g_fread_buffer[i]);
      switch (r) {
      case YXML_ELEMSTART:
        if (strcmp(x.elem, "item") == 0) {
          in_item = 1;
          in_title = 0;
          date_cache[0] = '\0';
        }
        if (strcmp(x.elem, "title") == 0) {
          in_title = 1;
          in_target_tag = TAG_TITLE;
        } else if (strcmp(x.elem, "link") == 0) {
          in_target_tag = TAG_LINK;
        } else if (strcmp(x.elem, "dc:date") == 0 ||
                   strcmp(x.elem, "pubDate") == 0 ||
                   strcmp(x.elem, "updated") == 0) {
          in_target_tag = TAG_DATE;
        } else if (strcmp(x.elem, "description") == 0 ||
                   strcmp(x.elem, "summary") == 0) {
          in_target_tag = TAG_SUMMARY;
        }
        buf_idx = 0;
        break;

      case YXML_CONTENT:
        if (in_target_tag) {
          // x.data has a UTF-8 character (1~4 bytes)
          for (uint8_t *p = x.data; *p && buf_idx < BUF_SIZE - 1; p++) {
            g_content_buffer[buf_idx++] = (uint8_t)*p;
          }
        }
        break;

      case YXML_ELEMEND:
        if (in_target_tag) {
          g_content_buffer[buf_idx] = '\0';

          // utf-8 to cp932
          convert_utf8_to_cp932(g_cp932_buffer, g_content_buffer, buf_idx);

          if (in_item) {
            if (in_target_tag == TAG_TITLE) {
              fprintf(fo, "\n%s\n", cp932rsc_horizontal_bar);
              fprintf(fo, "%%V%%W");
              smart_wrap_print(fo, g_cp932_buffer, 60);
              fprintf(fo, "%c\n", 0x18);
              if (date_cache[0] != '\0') {
                if (date_cache[0] == '2') {
                  fprintf(fo, "\n%s%.10s %.5s\n", cp932rsc_date, date_cache,
                          date_cache + 11);
                } else {
                  fprintf(fo, "\n%s%s\n", cp932rsc_date, date_cache);
                }
                date_cache[0] = '\0';
              }
            } else if (in_target_tag == TAG_DATE) {
              if (in_title == 0) {
                strcpy(date_cache, g_cp932_buffer);
              } else {
                if (g_cp932_buffer[0] == '2') {
                  fprintf(fo, "\n%s%.10s %.5s\n", cp932rsc_date, g_cp932_buffer,
                          g_cp932_buffer + 11);
                } else {
                  fprintf(fo, "\n%s%s\n", cp932rsc_date, g_cp932_buffer);
                }
              }
            } else if (in_target_tag == TAG_SUMMARY) {
              fputc('\n', fo);
              print_plain_text(fo, g_cp932_buffer);
              fputc('\n', fo);
            }
          } else {
            if (in_target_tag == TAG_TITLE) {
              fprintf(fo, "\n\n\n%%V%%W%s%c\n\n", g_cp932_buffer, 0x18);
            } else if (in_target_tag == TAG_SUMMARY) {
              fprintf(fo, "\n\n");
              print_plain_text(fo, g_cp932_buffer);
              fprintf(fo, "\n\n");
            }
          }
          in_target_tag = TAG_NONE;
        }
        break;

      case YXML_EEOF:
        rc = 0;
        goto exit;
      case YXML_EREF: {
        const uint8_t *ref = (const uint8_t *)x.string;
        uint8_t replacement = 0;

        if (strcmp(ref, "amp") == 0)
          replacement = '&';
        else if (strcmp(ref, "lt") == 0)
          replacement = '<';
        else if (strcmp(ref, "gt") == 0)
          replacement = '>';
        else if (strcmp(ref, "quot") == 0)
          replacement = '"';
        else if (strcmp(ref, "apos") == 0)
          replacement = '\'';

        if (replacement && in_target_tag && buf_idx < BUF_SIZE - 1) {
          g_content_buffer[buf_idx++] = replacement;
        }
      } break;
      case YXML_ATTRSTART:
        if (in_target_tag == 2 && strcmp(x.attr, "href") == 0) {
          buf_idx = 0; // preparation
        }
        break;
      case YXML_ATTRVAL:
        if (in_target_tag == 2) {
          for (uint8_t *p = x.data; *p && buf_idx < BUF_SIZE - 1; p++) {
            g_content_buffer[buf_idx++] = *p;
          }
        }
        break;
      default:
        if (r < 0) {
          fprintf(stderr, "XML Error at line %u\n", (unsigned int)x.line);
          rc = -1;
          goto exit;
        }
      }
    }
  }

exit:
  return rc;
}

//
//  helper: show help message
//
static void show_help_message() {
  printf("RSSNX.X - RSS reader & formatter X680x0/Human68k " VERSION " by tantan\n");
  printf("usage: rssnx [options] <rss-url> [output-file]\n");
  printf("options:\n");
  printf("     -h     ... show help message\n");
}

//
//  main
//
int32_t main(int32_t argc, uint8_t *argv[]) {

  // default return code
  int32_t rc = -1;

  // rss url
  uint8_t *rss_url = NULL;

  // output file name
  uint8_t output_file_name[MAX_FILENAME_LEN];
  strcpy(output_file_name, DEFAULT_OUTPUT_FILENAME); // default

  // input file handle
  FILE *fp = NULL;

  // output file handle
  FILE *fo = NULL;

  // parse command lines
  for (int16_t i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && strlen(argv[i]) >= 2) {
      if (argv[i][1] == 'h') {
        show_help_message();
        goto exit;
      } else {
        printf("error: unknown option (%s).\n", argv[i]);
        goto exit;
      }
    } else {
      if (rss_url == NULL) {
        rss_url = argv[i];
      } else {
        if (strlen(argv[i]) > MAX_FILENAME_LEN) {
          printf("error: too long output filename.\n");
          goto exit;
        }
        strcpy(output_file_name, argv[i]);
      }
    }
  }

  if (rss_url == NULL) {
    show_help_message();
    goto exit;
  }

try:

  // call wget
  int32_t exec_rc = spawnlp(P_WAIT, EXEC_WGET, EXEC_WGET, rss_url, "-O",
                            DEFAULT_DOWNLOAD_FILENAME, NULL);
  if (exec_rc != 0) {
    goto catch;
  }

  // output file handle
  fo = fopen(output_file_name, "wb");
  if (fo == NULL) {
    goto catch;
  }

  // input file handle
  fp = fopen(DEFAULT_DOWNLOAD_FILENAME, "rb");
  if (fp == NULL) {
    goto catch;
  }

  // parse RSS
  rc = parse_rss(fp, fo);

catch:

  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }

  if (fo != NULL) {
    fclose(fo);
    fo = NULL;
  }

  // remove input file
  DELETE(DEFAULT_DOWNLOAD_FILENAME);

  // remove output file in error cases
  if (rc != 0) {
    DELETE(output_file_name);
  }

exit:

  return rc;
}