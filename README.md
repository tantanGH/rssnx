# RSSNX

A native RSS Reader for Human68k + wget + DSHELL

## About This

**RSSNX** は、X680x0/Human68k 上でネイティブ動作する、RSS News Reader です。

- データ取得にyunk氏の **wget for x68k** を使用
- ユーザーインターフェイスとして電脳倶楽部の **DSHELL** を採用
- あらかじめ実績のあるいくつかのRSSサイトを定義済み

<img src='images/rssnx1.jpg' width='800px'/>

<br/>
<br/>

以下での動作を確認しています。

- X68030/060turbo実機 + イーサネットじょい君 + joynetd
- X68000XVI/PhantomX実機 + イーサネットじょい君 + joynetd

Nereid+inetd/hinetdでは動作未確認です。ご了承下さい。

---

## インストール方法

前提条件として、ネットワークに接続し、インターネットにアクセスできる状況になっていることが必要です。

yunk氏のイーサネットじょい君と`joynetd`の利用がとても簡単でお勧めです。

- https://yunkya2.booth.pm/items/8063175
- https://github.com/yunkya2/joynetd

また、同じくyunk氏の wget for x68k (`wget.x`) をパスの通ったディレクトリに導入しておいてください。
- https://github.com/yunkya2/wget-x68k

DSHELLの配布パッケージ `DSHL320E.LZH` をダウンロードして解凍し、`DSHELL.X` をパスの通ったディレクトリに置きます。すでに `wget.x` が利用可能であれば、以下のコマンドで直接68上でダウンロードすることができます。

```
wget https://github.com/tantanGH/rssnx/releases/download/v0.3.0/DSHL320E.LZH
```

`RSSNXxxx.ZIP` をダウンロードして解凍し、`RSSNX.X` をパスの通ったディレクトリに置きます。すでに `wget.x` が利用可能であれば、以下のコマンドで直接68上でダウンロードすることができます。

```
wget https://github.com/tantanGH/rssnx/releases/download/v0.3.0/RSSNX030.ZIP
```

---

## 起動方法および操作方法

以下のコマンドで、dshellを使って添付の`RSSNX.DOC`を開きます。

```
dshell RSSNX.DOC
```

DSHELLの基本操作：
- ◎クリックでリンクを辿る
- 左右同時クリックで戻る
- 左クリックで下方向へのスクロール
- 右クリックで上方向へのスクロール

RSSの各記事はセクションに分かれており、「改区」を押すことで記事単位で読み進めることもできます。

カレントディクレトリに作業ファイルとして `_D.D` `_R.D` という名前のファイルが作られますのでご了承ください。

---

## RSSサイト追加方法

いくつかのメジャーなRSSサイトで、かつ現状の`wget.x`でアクセス可能なものはあらかじめいくつか `RSSNX.DOC` 内に定義されています。

`RSSNX.DOC`はDSHELLマークアップ形式のテキストファイルですので、`ED.X`などで編集してご自身で追加することも可能です。ただしリンクの書式は以下に従ってください。

`TYPE=EDE:RSSNX (RSSフィードURL);_R.D`

例：
```
TYPE=EDE:RSSNX https://akiba-pc.watch.impress.co.jp/data/rss/1.0/ah/feed.rdf;_R.D
```

現時点で`wget.x`はあらゆるサイトのhttps接続に対応しているわけではありませんので、データの取得ができない場合もあります。
また、対応している場合でもタイミングによっては接続に失敗することがあります。その場合は再度リンクをクリックするなどリトライしてください。

---

## Copyright / 謝辞

XMLパーサーとして、yxmlを利用させていただいております。

RSSNXのビルドは現状は elf2x68k と xdev68k のどちらでも可能となっています。(2つのMakefileがツリーに存在します) 配布しているバイナリは elf2x68kでビルドしたものになります。重ね重ねyunk氏のご尽力に感謝します。


  Copyright (c) 2013-2014 Yoran Heling

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

## 変更履歴

version 0.3.0 (2026.03.19) ... 初版