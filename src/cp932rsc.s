  .align 16

  .globl _cp932rsc_date
  .globl _cp932rsc_horizontal_bar
  .globl _cp932rsc_space

  .data

_cp932rsc_date:
  .dc.b '日付:',$00

_cp932rsc_horizontal_bar:
  .dc.b $84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa
  .dc.b $84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa
  .dc.b $84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa
  .dc.b $84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa
  .dc.b $84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa,$84,$aa
  .dc.b $00

_cp932rsc_space:
  .dc.b $81,$40,$00

  .end