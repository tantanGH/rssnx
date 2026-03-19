#
#  xdev68k用 Makefile
#

# 必要な環境変数が定義されていることを確認する。
ifndef XDEV68K_DIR
	XDEV68K_DIR=/opt/xdev68k
endif

# デフォルトサフィックスを削除
.SUFFIXES:

# 各種コマンド短縮名
CXX = ${XDEV68K_DIR}/m68k-toolchain/bin/m68k-elf-g++
CC = ${XDEV68K_DIR}/m68k-toolchain/bin/m68k-elf-gcc
GAS2HAS = perl ${XDEV68K_DIR}/util/x68k_gas2has.pl -cpu 68000 -inc doscall.inc -inc iocscall.inc
RUN68 = ${XDEV68K_DIR}/run68/run68x
HAS = $(RUN68) ${XDEV68K_DIR}/x68k_bin/HAS060.X
HLK = $(RUN68) ${XDEV68K_DIR}/x68k_bin/hlk.r

# 実行ファイル名
TARGET_FILE = RSSNX.X

# Resource files
RESOURCE_FILES = RSSNX.TXT RSSNX.DOC

# Distribution package 
PACKAGE_FILE = ../RSSNX020.ZIP

# ヘッダ検索パス
INCLUDE_FLAGS = -I${XDEV68K_DIR}/include/xc -I${XDEV68K_DIR}/include/xdev68k

# コンパイルフラグ
COMMON_FLAGS = -m68000 -msoft-float -O2 $(INCLUDE_FLAGS)
CFLAGS = $(COMMON_FLAGS) -Wno-builtin-declaration-mismatch -fcall-used-d2 -fcall-used-a2 \
    -fexec-charset=cp932 -fverbose-asm

# *.c ソースファイル
C_SRCS = utf8_cp932.c yxml.c main.c

# *.s ソースファイル
ASM_SRCS = cp932rsc.s

# *.h source files
HEADER_SRCS = cp932rsc.h utf8_cp932.h yxml.h rssnx.h

# リンク対象のライブラリファイル
LIBS =\
	${XDEV68K_DIR}/lib/xc/CLIB.L \
	${XDEV68K_DIR}/lib/xc/DOSLIB.L \
	${XDEV68K_DIR}/lib/xc/IOCSLIB.L \
	${XDEV68K_DIR}/lib/xc/FLOATFNC.L \
	${XDEV68K_DIR}/lib/m68k_elf/m68000/libgcc.a

# 中間ファイル生成用ディレクトリ
INTERMEDIATE_DIR = _build

# オブジェクトファイル
OBJS =	$(addprefix $(INTERMEDIATE_DIR)/,$(patsubst %.c,%.o,$(C_SRCS))) \
	$(addprefix $(INTERMEDIATE_DIR)/,$(patsubst %.s,%.o,$(ASM_SRCS)))

# HLK に入力するリンクリスト
HLK_LINK_LIST = $(INTERMEDIATE_DIR)/_lk_list.tmp

# デフォルトのターゲット
all : ${INTERMEDIATE_DIR}/$(TARGET_FILE) ${INTERMEDIATE_DIR}/$(TARGET_FILE2) 

# 中間生成物の削除
clean : 
	rm -f $(TARGET_FILE)
	rm -rf $(INTERMEDIATE_DIR)

${INTERMEDIATE_DIR}/$(TARGET_FILE) : $(OBJS)
	mkdir -p $(INTERMEDIATE_DIR)
	rm -f $(HLK_LINK_LIST)
	@for FILENAME in $(OBJS); do\
		echo $$FILENAME >> $(HLK_LINK_LIST); \
        done
	@for FILENAME in $(LIBS); do\
		cp $$FILENAME $(INTERMEDIATE_DIR)/`basename $$FILENAME`; \
		echo $(INTERMEDIATE_DIR)/`basename $$FILENAME` >> $(HLK_LINK_LIST); \
        done
	$(HLK) -i $(HLK_LINK_LIST) -o ${INTERMEDIATE_DIR}/$(TARGET_FILE)
	rm -f tmp*.\$$\$$\$$

# *.c ソースのコンパイル
$(INTERMEDIATE_DIR)/%.o : %.c Makefile $(HEADER_SRCS) $(HEADER_SRCS2)
	mkdir -p $(INTERMEDIATE_DIR)
	$(CC) -S $(CFLAGS) -o $(INTERMEDIATE_DIR)/$*.m68k-gas.s $<
	$(GAS2HAS) -i $(INTERMEDIATE_DIR)/$*.m68k-gas.s -o $(INTERMEDIATE_DIR)/$*.s
	rm -f $(INTERMEDIATE_DIR)/$*.m68k-gas.s
	$(HAS) -e -u -w0 $(INCLUDE_FLAGS) $(INTERMEDIATE_DIR)/$*.s -o $(INTERMEDIATE_DIR)/$*.o

# *.s ソースのアセンブル
$(INTERMEDIATE_DIR)/%.o : %.s Makefile
	mkdir -p $(INTERMEDIATE_DIR)
	iconv -t cp932 $*.s > $(INTERMEDIATE_DIR)/$*.has
	$(HAS) -e -u -w0 $(INCLUDE_FLAGS) $(INTERMEDIATE_DIR)/$*.has -o $(INTERMEDIATE_DIR)/$*.o

package:
	zip -j ${PACKAGE_FILE} ${INTERMEDIATE_DIR}/${TARGET_FILE} ${RESOURCE_FILES} 
