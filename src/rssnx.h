#ifndef __H_RSSNX__
#define __H_RSSNX__

#define VERSION "0.3.0 (2026/03/19)"

#define MAX_FILENAME_LEN (250)

#define EXEC_WGET "WGET.X"

#define DEFAULT_DOWNLOAD_FILENAME "_D.D"
#define DEFAULT_OUTPUT_FILENAME   "_R.D"

#define BUF_SIZE        (32768)
#define STACK_SIZE      (4096)
#define FREAD_BUF_SIZE  (32768)

#define TAG_NONE        (0)
#define TAG_TITLE       (1)
#define TAG_LINK        (2)
#define TAG_DATE        (3)
#define TAG_SUMMARY     (4)

#endif