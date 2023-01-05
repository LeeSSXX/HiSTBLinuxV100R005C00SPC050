" Menu Translations:	Turkish
" Maintainer:			Emir SARI <bitigchi@me.com>

if exists("did_menu_trans")
   finish
endif
let did_menu_trans = 1
let s:keepcpo= &cpo
set cpo&vim

scriptencoding latin1 

" Top
menutrans &File				&Dosya
menutrans &Edit				D�&zen
menutrans &Tools			&Ara�lar
menutrans &Syntax			&S�zdizim
menutrans &Buffers			A&rabellekler
menutrans &Window			&Pencere
menutrans &Help				&Yardim


" Help menu
menutrans &Overview<Tab><F1>	&Genel\ Bakis<Tab><F1>
menutrans &User\ Manual			&Kullanim\ Kilavuzu
menutrans &How-To\ Links		&Nasil\ Yapilir?
menutrans &Find\.\.\.			&Bul\.\.\.
"--------------------
menutrans &Credits			    &Tesekk�rler
menutrans Co&pying			    &Dagitim
menutrans &Sponsor/Register		&Sponsorluk/Kayit
menutrans O&rphans			    &Yetimler
"--------------------
menutrans &Version			    S�r�m\ &Bilgisi
menutrans &About			    &Hakkinda


" File menu
menutrans &Open\.\.\.<Tab>:e		    &A�\.\.\.<Tab>:e
menutrans Sp&lit-Open\.\.\.<Tab>:sp	    &Yeni\ B�l�mde\ A�\.\.\.<Tab>:sp
menutrans Open\ Tab\.\.\.<Tab>:tabnew	S&ekme\ A�\.\.\.<Tab>:tabnew
menutrans &New<Tab>:enew		        Yeni\ &Sekme<Tab>:enew
menutrans &Close<Tab>:close		        Ka&pat<Tab>:close
"--------------------
menutrans &Save<Tab>:w			        Ka&ydet<Tab>:w
menutrans Save\ &As\.\.\.<Tab>:sav	    &Farkli Kaydet\.\.\.<Tab>:sav
"--------------------
menutrans Split\ &Diff\ With\.\.\.	    Ka&rsilastir\.\.\.
menutrans Split\ Patched\ &By\.\.\.	    Ya&malar\ Dahil\ Karsilastir\.\.\.
"--------------------
menutrans &Print			            Ya&zdir
menutrans Sa&ve-Exit<Tab>:wqa		    Kaydet\ &ve �ik<Tab>:wqa
menutrans E&xit<Tab>:qa			        �i&k<Tab>:qa


" Edit menu
menutrans &Undo<Tab>u			&Geri\ Al<Tab>u
menutrans &Redo<Tab>^R			&Yinele<Tab>^R
menutrans Rep&eat<Tab>\.		Son\ Komutu\ Y&inele<Tab>\.
"--------------------
menutrans Cu&t<Tab>"+x			&Kes<Tab>"+x
menutrans &Copy<Tab>"+y			K&opyala<Tab>"+y
menutrans &Paste<Tab>"+gP		Ya&pistir<Tab>"+gP
menutrans Put\ &Before<Tab>[p	�&n�ne Koy<Tab>[p
menutrans Put\ &After<Tab>]p	A&rkasina Koy<Tab>]p
menutrans &Delete<Tab>x			Si&l<Tab>x
menutrans &Select\ All<Tab>ggVG	T�&m�n�\ Se�<Tab>ggVG
"--------------------
" Athena GUI only
menutrans &Find<Tab>/			            &Bul<Tab>/
menutrans Find\ and\ Rep&lace<Tab>:%s	    Bul\ &ve\ Degistir<Tab>:%s
" End Athena GUI only
menutrans &Find\.\.\.<Tab>/		            &Bul\.\.\.<Tab>/
menutrans Find\ and\ Rep&lace\.\.\.	        Bul\ ve\ &Degistir\.\.\.
menutrans Find\ and\ Rep&lace\.\.\.<Tab>:%s	Bul\ ve\ &Degistir\.\.\.<Tab>:%s
menutrans Find\ and\ Rep&lace\.\.\.<Tab>:s	Bul\ ve\ &Degistir\.\.\.<Tab>:s
"--------------------
menutrans Settings\ &Window		&Ayarlar\ Penceresi
menutrans Startup\ &Settings	Baslan&gi�\ Ayarlari
menutrans &Global\ Settings		Ge&nel\ Ayarlar
menutrans F&ile\ Settings		&Dosya\ Ayarlari
menutrans C&olor\ Scheme		&Renk\ D�zeni
menutrans &Keymap			    D�gme\ &Eslem
menutrans Select\ Fo&nt\.\.\.	Ya&zitipi Se�\.\.\.
">>>----------------- Edit/Global settings
menutrans Toggle\ Pattern\ &Highlight<Tab>:set\ hls!        Dizgi\ &Vurgulamasini\ A�/Kapat<Tab>:set\ hls!
menutrans Toggle\ &Ignoring\ Case<Tab>:set\ ic!		        B�Y�K/k���k\ Harf\ &Duyarsiz\ A�/Kapat<Tab>:set\ ic!
menutrans Toggle\ &Showing\ Matched\ Pairs<Tab>:set\ sm!	Es&lesen\ Ikilileri\ A�/Kapat<Tab>:set\ sm!
menutrans &Context\ Lines				                    I&mle�le\ Oynayan\ Satirlar
menutrans &Virtual\ Edit				                    &Sanal\ D�zenleme
menutrans Toggle\ Insert\ &Mode<Tab>:set\ im!		        Ekleme\ &Kipini\ A�/Kapat<Tab>:set\ im!
menutrans Toggle\ Vi\ C&ompatibility<Tab>:set\ cp!		    &Vi\ Uyumlu\ Kipi\ A�/Kapat<Tab>:set\ cp!
menutrans Search\ &Path\.\.\.				                &Arama\ Yolu\.\.\.
menutrans Ta&g\ Files\.\.\.				                    &Etiket\ Dosyalari\.\.\.
"
menutrans Toggle\ &Toolbar				    &Ara�\ �ubugunu\ A�/Kapat
menutrans Toggle\ &Bottom\ Scrollbar		A&lt\ Kaydirma\ �ubugunu\ A�/Kapat
menutrans Toggle\ &Left\ Scrollbar			&Sol\ Kaydirma\ �ubugunu\ A�/Kapat
menutrans Toggle\ &Right\ Scrollbar		    S&ag\ Kaydirma\ �ubugunu\ A�/Kapat  	
">>>->>>------------- Edit/Global settings/Virtual edit
menutrans Never					Kapali
menutrans Block\ Selection		Blok\ Se�imi
menutrans Insert\ Mode			Ekleme\ Kipi
menutrans Block\ and\ Insert	Blok\ Se�iminde\ ve\ Ekleme\ Kipinde
menutrans Always				Her\ Zaman\ A�ik
">>>----------------- Edit/File settings
menutrans Toggle\ Line\ &Numbering<Tab>:set\ nu!	        &Satir\ Numaralandirmayi\ A�/Kapat<Tab>:set\ nu!
menutrans Toggle\ Relati&ve\ Line\ Numbering<Tab>:set\ rnu!	&G�receli\ Satir\ Numaralandirmayi\ A�/Kapat<Tab>:set\ nru!
menutrans Toggle\ &List\ Mode<Tab>:set\ list!		        G�&r�nmeyen\ Karakterleri\ A�/Kapat<Tab>:set\ list!
menutrans Toggle\ Line\ &Wrapping<Tab>:set\ wrap!	        Sa&tir\ Kaydirmayi\ A�/Kapat<Tab>:set\ wrap!
menutrans Toggle\ W&rapping\ at\ Word<Tab>:set\ lbr!	    S�&zc�k\ Kaydirmayi\ A�/Kapat<Tab>:set\ lbr!
menutrans Toggle\ Tab\ &Expanding-tab<Tab>:set\ et!	        S&ekmeleri\ Bosluklara\ D�n�st�rmeyi\ A�/Kapat<Tab>:set\ et!
menutrans Toggle\ &Auto\ Indenting<Tab>:set\ ai!	        &Otomatik\ Girintilemeyi\ A�/Kapat<Tab>:set\ ai!
menutrans Toggle\ &C-Style\ Indenting<Tab>:set\ cin!	    &C\ Tarzi\ Girintilemeyi\ A�/Kapat<Tab>:set\ cin!
">>>---
menutrans &Shiftwidth			&Girinti\ D�zeyi
menutrans Soft\ &Tabstop		&Sekme\ Genisligi
menutrans Te&xt\ Width\.\.\.	&Metin\ Genisligi\.\.\.
menutrans &File\ Format\.\.\.	&Dosya\ Bi�imi\.\.\.
"
"
"
" Tools menu
menutrans &Jump\ to\ This\ Tag<Tab>g^]		S&u\ Etikete\ Atla<Tab>g^]
menutrans Jump\ &Back<Tab>^T				&Geri\ D�n<Tab>^T
menutrans Build\ &Tags\ File				&Etiket\ Dosyasi\ Olustur
"-------------------
menutrans &Folding					&Kivirmalar
menutrans &Spelling					&Yazim\ Denetimi
menutrans &Diff						&Ayrimlar\ (diff)
"-------------------
menutrans &Make<Tab>:make						&Derle<Tab>:make
menutrans &List\ Errors<Tab>:cl					&Hatalari\ Listele<Tab>:cl
menutrans L&ist\ Messages<Tab>:cl!				I&letileri\ Listele<Tab>:cl!
menutrans &Next\ Error<Tab>:cn					Bir\ &Sonraki\ Hata<Tab>:cn
menutrans &Previous\ Error<Tab>:cp				Bir\ �&nceki\ Hata<Tab>:cp
menutrans &Older\ List<Tab>:cold				Daha\ &Eski\ Hatalar<Tab>:cold
menutrans N&ewer\ List<Tab>:cnew				Daha\ &Yeni\ Hatalar<Tab>:cnew
menutrans Error\ &Window						Hatalar\ &Penceresi
menutrans Se&t\ Compiler						De&rleyici\ Se�
menutrans Show\ Compiler\ Se&ttings\ in\ Menu	Derleyici\ Ayarlarini\ Men�de\ &G�ster 
"-------------------
menutrans &Convert\ to\ HEX<Tab>:%!xxd			HEX'e\ D�&n�st�r<Tab>:%!xxd
menutrans Conve&rt\ Back<Tab>:%!xxd\ -r			HEX'&ten\ D�n�st�r<Tab>:%!xxd\ -r
">>>---------------- Tools/Spelling
menutrans &Spell\ Check\ On						Yazim\ Denetimini\ &A�
menutrans Spell\ Check\ &Off					Yazim\ Denetimini\ &Kapat
menutrans To\ &Next\ Error<Tab>]s				Bir\ &Sonraki\ Hata<Tab>]s
menutrans To\ &Previous\ Error<Tab>[s			Bir\ �&nceki\ Hata<Tab>[s
menutrans Suggest\ &Corrections<Tab>z=			D�&zeltme\ �ner<Tab>z=
menutrans &Repeat\ Correction<Tab>:spellrepall	D�zeltmeyi\ &Yinele<Tab>spellrepall
"-------------------
menutrans Set\ Language\ to\ "en"		Dili\ "en"\ yap
menutrans Set\ Language\ to\ "en_au"	Dili\ "en_au"\ yap
menutrans Set\ Language\ to\ "en_ca"	Dili\ "en_ca"\ yap
menutrans Set\ Language\ to\ "en_gb"	Dili\ "en_gb"\ yap
menutrans Set\ Language\ to\ "en_nz"	Dili\ "en_nz"\ yap
menutrans Set\ Language\ to\ "en_us"	Dili\ "en_us"\ yap
menutrans &Find\ More\ Languages		&Baska\ Diller\ Bul
let g:menutrans_set_lang_to =			'Dil Y�kle'
"
"
" The Spelling popup menu
"
"
let g:menutrans_spell_change_ARG_to =			'D�zeltilecek:\ "%s"\ ->'
let g:menutrans_spell_add_ARG_to_word_list =	'"%s"\ s�zc�g�n�\ s�zl�ge\ ekle'
let g:menutrans_spell_ignore_ARG =				'"%s"\ s�zc�g�n�\ yoksay'
">>>---------------- Folds
menutrans &Enable/Disable\ Folds<Tab>zi			&Kivirmalari\ A�/Kapat<Tab>zi
menutrans &View\ Cursor\ Line<Tab>zv			I&mlecin\ Oldugu\ Satiri\ G�r�nt�le<Tab>zv
menutrans Vie&w\ Cursor\ Line\ Only<Tab>zMzx	Ya&lnizca\ Imlecin\ Oldugu\ Satiri\ G�r�nt�le<Tab>zMzx
menutrans C&lose\ More\ Folds<Tab>zm			&Daha\ Fazla\ Kivirma\ Kapat<Tab>zm
menutrans &Close\ All\ Folds<Tab>zM				B�t�n\ Ki&virmalari\ Kapat<Tab>zM
menutrans &Open\ All\ Folds<Tab>zR				B�&t�n\ Kivirmalari\ A�<Tab>zR
menutrans O&pen\ More\ Folds<Tab>zr				D&aha\ Fazla\ Kivirma\ A�<Tab>zr
menutrans Fold\ Met&hod							Kivi&rma\ Y�ntemi
menutrans Create\ &Fold<Tab>zf					Kivirma\ &Olustur<Tab>zf
menutrans &Delete\ Fold<Tab>zd					Kivirma\ &Sil<Tab>zd
menutrans Delete\ &All\ Folds<Tab>zD			T�&m\ Kivirmalari\ Sil<Tab>zD
menutrans Fold\ col&umn\ Width					Kivirma\ S�tunu\ &Genisligi
">>>->>>----------- Tools/Folds/Fold Method
menutrans M&anual		&El\ Ile
menutrans I&ndent		&Girinti
menutrans E&xpression	I&fade
menutrans S&yntax		&S�zdizim
menutrans Ma&rker		I&mleyici
">>>--------------- Tools/Diff
menutrans &Update		&G�ncelle
menutrans &Get\ Block	Blogu\ &Al
menutrans &Put\ Block	Blogu\ &Koy
">>>--------------- Tools/Diff/Error window
menutrans &Update<Tab>:cwin		&G�ncelle<Tab>:cwin
menutrans &Close<Tab>:cclose	&Kapat<Tab>:cclose
menutrans &Open<Tab>:copen		&A�<Tab>:copen
"
"
" Syntax menu
"
menutrans &Show\ File\ Types\ in\ Menu		Dosya\ T�rlerini\ Men�de\ &G�ster
menutrans Set\ '&syntax'\ only				Yalnizca\ 'syntax'\ &Ayarla
menutrans Set\ '&filetype'\ too				'filetype'\ I�in\ &de\ Ayarla
menutrans &Off								&Kapat
menutrans &Manual							&El\ Ile
menutrans A&utomatic						&Otomatik
menutrans On/Off\ for\ &This\ File			&Bu\ Dosya\ I�in\ A�/Kapat
menutrans Co&lor\ Test						&Renk\ Testi
menutrans &Highlight\ Test					&Vurgulama\ Testi
menutrans &Convert\ to\ HTML				&HTML'ye\ D�n�st�r
"
"
" Buffers menu
"
menutrans &Refresh\ menu	&Men�y�\ G�ncelle
menutrans Delete			&Sil
menutrans &Alternate		�&teki
menutrans &Next				So&nraki
menutrans &Previous			�n&ceki
menutrans [No\ File]		[Dosya\ Yok]
"
"
" Window menu
"
menutrans &New<Tab>^Wn					Yeni\ &Pencere<Tab>^Wn
menutrans S&plit<Tab>^Ws				Pencereyi\ &B�l<Tab>^Ws
menutrans Sp&lit\ To\ #<Tab>^W^^		Pencereyi\ Baskasina\ B�&l<Tab>^W^^
menutrans Split\ &Vertically<Tab>^Wv	Pencereyi\ &Dikey\ Olarak\ B�l<Tab>^Wv
menutrans Split\ File\ E&xplorer		Yeni\ B�l�&mde\ Dosya\ Gezginini\ A�
"
menutrans &Close<Tab>^Wc				Pen&cereyi\ Kapat<Tab>^Wc
menutrans Close\ &Other(s)<Tab>^Wo		Diger\ Pencerele&ri\ Kapat<Tab>^Wo
"
menutrans Move\ &To						&Tasi
menutrans Rotate\ &Up<Tab>^WR			&Yukari\ Tasi<Tab>^WR
menutrans Rotate\ &Down<Tab>^Wr			&Asagi\ Tasi<Tab>^Wr
"
menutrans &Equal\ Size<Tab>^W=			&Esit\ Boyut<Tab>^W=
menutrans &Max\ Height<Tab>^W_			E&n\ B�y�k\ Y�kseklik<Tab>^W_
menutrans M&in\ Height<Tab>^W1_			En\ K���k\ Y�ksekl&ik<Tab>^W1_
menutrans Max\ &Width<Tab>^W\|			En\ B�y�k\ Gen&islik<Tab>^W\|
menutrans Min\ Widt&h<Tab>^W1\|			En\ K���k\ Genis&lik<Tab>^W1\|
">>>----------------- Window/Move To
menutrans &Top<Tab>^WK					&Yukari<Tab>^WK
menutrans &Bottom<Tab>^WJ				&Asagi<Tab>^WJ
menutrans &Left\ Side<Tab>^WH			So&la<Tab>^WH
menutrans &Right\ Side<Tab>^WL			&Saga<Tab>^WL
"
"
" The popup menu
"
"
menutrans &Undo					&Geri\ Al
menutrans Cu&t					&Kes
menutrans &Copy					K&opyala
menutrans &Paste				&Yapistir
menutrans &Delete				&Sil
menutrans Select\ Blockwise		&Blok\ Bi�iminde\ Se�
menutrans Select\ &Word			S�&zc�k\ Se�
menutrans Select\ &Sentence		&T�mce\ Se�
menutrans Select\ Pa&ragraph	&Paragraf\ Se�
menutrans Select\ &Line			S&atir\ Se�
menutrans Select\ &Block		Bl&ok\ Se�
menutrans Select\ &All			T�m�&n�\ Se�
"
" The GUI toolbar
"
if has("toolbar")
  if exists("*Do_toolbar_tmenu")
    delfun Do_toolbar_tmenu
  endif
  fun Do_toolbar_tmenu()
    tmenu ToolBar.Open			Dosya A�
    tmenu ToolBar.Save			Dosya Kaydet
    tmenu ToolBar.SaveAll		T�m Dosyalari Kaydet
    tmenu ToolBar.Print			Yazdir
    tmenu ToolBar.Undo			Geri Al
    tmenu ToolBar.Redo			Yinele
    tmenu ToolBar.Cut			Kes
    tmenu ToolBar.Copy			Kopyala
    tmenu ToolBar.Paste			Yapistir
	tmenu ToolBar.Find			Bul...
    tmenu ToolBar.FindNext		Sonrakini Bul
    tmenu ToolBar.FindPrev		�ncekini Bul
    tmenu ToolBar.Replace		Bul ve Degistir...
    if 0	" disabled; These are in the Windows menu
      tmenu ToolBar.New			Yeni Pencere
      tmenu ToolBar.WinSplit	Pencereyi B�l
      tmenu ToolBar.WinMax		En B�y�k Pencere Y�ksekligi
      tmenu ToolBar.WinMin		En K���k Pencere Y�ksekligi
      tmenu ToolBar.WinClose	Pencereyi Kapat
    endif
    tmenu ToolBar.LoadSesn		Oturum Y�kle
    tmenu ToolBar.SaveSesn		Oturum Kaydet
    tmenu ToolBar.RunScript		Betik �alistir
    tmenu ToolBar.Make			Derle
    tmenu ToolBar.Shell			Kabuk
    tmenu ToolBar.RunCtags		Etiket Dosyasi Olustur
    tmenu ToolBar.TagJump		Etikete Atla
    tmenu ToolBar.Help			Yardim
    tmenu ToolBar.FindHelp		Yardim Bul
  endfun
endif
"
"
" Dialog texts
"
" Find in help dialog
"
let g:menutrans_help_dialog = "Yardim icin komut veya sozcuk girin:\n\nEkleme Kipi komutlarini aramak icin i_ ekleyin (ornegin i_CTRL-X)\nNormal Kip komutlarini aramak icin _c ekleyin (ornegin c_<Del>)\nSecenekler hakkinda yardim almak icin ' ekleyin (ornegin 'shiftwidth')"
"
"
" Searh path dialog
"
let g:menutrans_path_dialog = "Dosya aramasi i�in yol belirtin.\nDizin adlari virg�llerle ayrilir."
"
" Tag files dialog
"
let g:menutrans_tags_dialog = "Etiket dosyasi adlari belirtin (virg�lle ayirarak).\n"
"
" Text width dialog
"
let g:menutrans_textwidth_dialog = "Bi�imlendirme i�in metin genisligini belirtin.\nBi�imlendirme iptali i�in 0 girin."
"
" File format dialog
"
let g:menutrans_fileformat_dialog = "Dosya bi�imi se�in"
let g:menutrans_fileformat_choices = "&Unix\n&Dos\n&Mac\nI&ptal"
"
let menutrans_no_file = "[Dosya Yok]"

let &cpo = s:keepcpo
unlet s:keepcpo
