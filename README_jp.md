# LoRa-BASIC for LRA1 Firmware

[日本語](README_jp.md) | [English](README.md)

**LoRa-BASICは、LRA1 LoRaモジュールをBASICでプログラムするためのファームウェアです。LoRa-P2P通信とLoRaWAN通信の両方を、BASICから直接制御できます。**

LoRa-P2Pによるパケット送受信、LoRaWANネットワークへのJoinやデータ送受信、UART・I2C・SPIなどの周辺機能制御、アプリケーション処理まで、外部MCUを使用せずLRA1単体で実行できます。

シリアル端末からREPLでコマンドを対話的に実行できるほか、行番号付きのBASICプログラムを入力し、`RUN` コマンドで実行することもできます。

“わずか数行のBASICで、LoRaWANのデーターを送信可能。”

> **ソースコードについて**  
> 本コードは長年の機能追加や保守を重ねてきたため、継ぎ足しになっている部分や、あまり整理されていない箇所があります。  
> 実際に動作している実装および参考資料として公開するものですので、その点を割り引いてご覧ください。

## LRA1と開発について

LRA1は、i2-electronicsが設計・製造・販売するLoRa通信モジュールです。ハードウェアの詳細は、[i2-electronicsの製品情報](https://i2-ele.co.jp/LoRa.html)を参照してください。

本ファームウェアは、LRA1向けに**Shachi-labが設計・実装**したものです。LRA1に搭載されているファームウェアを、引き続きShachi-lab名義で開発・公開しています。

BASICインタプリタは、同じくShachi-labが開発したnanoBASICを基に、LoRa-BASIC向けに拡張・保守しています。Microchip ASFやSemtech LoRaMac-nodeなどの第三者コードを利用しており、それぞれのライセンスは保持しています。

## 地域・無線認証について

LoRa-BASIC for LRA1 は、日本国内での使用を想定して設計されています。  
LoRaWANモードは AS923-1 のリージョナルパラメータに対応し、P2Pモードも日本の920MHz帯での使用を前提とした周波数およびチャンネル設定となっています。

LRA1モジュールは、日本国内の電波法に基づく技術基準適合証明を取得しています。  
本リポジトリで公開しているファームウェアは、この認証条件の範囲内で動作することを前提としています。

ただし、ソースコードを変更して送信周波数、送信出力、送信時間、キャリアセンスなど無線動作に関係する仕様を変更した場合、技術基準適合証明の条件を満たさなくなる可能性があります。改造したファームウェアを実機で使用する場合は、適用される法令および認証条件を利用者自身で確認してください。

日本国外での使用、および各国・地域の無線規制や認証への適合についてはサポートしていません。
## できること

- **BASICから無線通信**：LoRaプライベート通信（P2P）とLoRaWANに対応。
- **外付けMCUなしで処理と通信**：簡単な動作なら、モジュール内のBASICプログラムだけで実行。
- **周辺機器との接続**：UART・I2C・SPIをBASICから利用。
- **REPLで対話的に実行**：シリアル端末からBASICの文やコマンドを直接入力し、その場で実行結果を確認できます。
- **BASICプログラムを作成して実行**：REPLから行番号付きのBASICプログラムを入力・編集し、`RUN` コマンドで実行できます。

## マニュアル

用途別に4冊のリファレンスマニュアルを用意しています。日本語版を原典とし、英語版も公開しています。

コマンド、変数、関数をすばやく探す場合は、[日本語一覧](docs/lora-basic-command-reference_jp.md)または[英語一覧](docs/lora-basic-command-reference.md)を参照してください。

| マニュアル | 日本語版 | 英語版 | 内容 |
|---|---|---|---|
| 基本編 | [日本語](docs/lora-basic-manual_basics_jp.md) | [English](docs/lora-basic-manual_basics_en.md) | 接続、操作モード、BASICの文法、基本コマンド、入出力、システム変数、UART2・I2C・SPIなど |
| P2P編 | [日本語](docs/lora-basic-manual_p2p_jp.md) | [English](docs/lora-basic-manual_p2p_en.md) | LoRa-P2P通信の設定、送受信、通信バッファ、キャリアセンス、転送機能など |
| LoRaWAN編 | [日本語](docs/lora-basic-manual-wan_jp.md) | [English](docs/lora-basic-manual-wan_en.md) | LoRaWANへの接続、OTAA・ABP、アップリンク・ダウンリンク、関連コマンドと変数など |
| メンテナンス編 | [日本語](docs/lora-basic-manual-maintenance_jp.md) | [English](docs/lora-basic-manual-maintenance_en.md) | 製造時の調整・検査で使用するメンテナンスレベルのコマンドと変数 |

使用方法、接続・通信設定、コマンド、文法の詳細は、対応するマニュアルを参照してください。  
以下は動作のイメージを紹介する例です。

### REPLモードの例：VDD電圧を計測してP2P送信

VDD電圧を計測して値を確認し、値を1/100にしてP2Pで送信します。プログラム全体を書かずに、一つずつ試せます。

```basic
>a=adc(27)
OK
>?a
3332
OK
>b=a/100
OK
>send b
OK
```

### BASICプログラムの例：評価ボードからLoRaWANで送信

LRA1の評価ボードで、気温・湿度・気圧・電圧を取得してLoRaWANで送信する例です。センサーの読み取り、送信データの設定、表示、無線送信までを、次のBASICプログラムで実行できます。

```basic
100 Do
110 If !Wan_JoinStat Then Wan_Join EndIf
120 If Wan_JoinStat Then
130 Bme T,H,P
140 V=Adc(27)
150 Txd[7]=8
160 Txdw[8]=T:Txdw[10]=H:Txdw[12]=P:Txdw[14]=V
170 Lpos=0:Lprint 0(T/10,2)"."T%10" "0(H/10,2)"%"
180 Lpos=64:Lprint 0(P/10,4)"*"V/10
190 Wan_Tx
200 EndIf
210 Sleep 600
220 Loop
```

この例では、説明を簡潔にするためエラー処理を省略しています。

## 公開対象

このリポジトリは、**Shachi-lab版 v1.28.a以降**のファームウェアのソースコードとバイナリ（.bin）を対象とします。

**v1.28.aより前のバージョンは範囲外です。** 旧版が必要な場合は、[i2-electronicsのWebサイト](https://i2-ele.co.jp/LoRa.html)で配布されているバイナリを使用してください。変更履歴は[CHANGELOG_jp.md](CHANGELOG_jp.md)を参照してください。

## ファームウェアのダウンロード

公開済みのファームウェア（`.bin`）は、[GitHub Releases](https://github.com/shachi-lab/lora-basic-for-lra1/releases)から取得できます。必要なバージョンを選び、添付されたバイナリファイルをダウンロードしてください。

## ファームウェアのアップデート

Shachi-labが別途公開している**[Python版アップデーター](https://github.com/shachi-lab/lra1_tool)**を使用してください。セットアップと操作方法は、アップデーターのREADMEを参照してください。

| アップデーター | Shachi-lab版バイナリ | i2-electronics配布バイナリ |
|---|---|---|
| Shachi-lab公開のPython版 | 書き込み可能 | 書き込み可能 |
| i2-electronics配布版（Windows版・Python版を含む） | 書き込み不可（エラー） | 書き込み可能 |

**i2-electronics配布のツールでは、Shachi-lab版へアップデートできません。** Shachi-lab公開のPython版では、i2-electronics配布のバイナリも書き込めます。

ブートローダーは**別リポジトリで後日公開予定**です。LRA1の工場出荷時に書き込まれているブートローダーと互換性があります。

## ファームウェアのビルドとソース構成

ファームウェア自体をソースコードからビルドするには、**Microchip Studio 7**が必要です。`LoRa_Basic_SAMR35/LoRa_Basic_SAMR35.atsln`を開いてください。内蔵BASICを使うだけなら、Microchip Studioは不要です。

主要な構成を示します。下表のプロジェクト内パスは、`LoRa_Basic_SAMR35/`からの相対パスです。

| プロジェクト内パス | 内容 |
|---|---|
| `LoRa_Basic_SAMR35.cproj` | Microchip Studioのプロジェクト設定 |
| `src/apps/` | BASICインタプリタ、LoRaWAN／P2P制御など |
| `src/boards/` | LRA1／SAMR35向けのボード依存処理 |
| `src/mac/`、`src/radio/` | LoRaMAC、無線ドライバー |
| `src/system/`、`src/peripherals/`、`src/config/` | システム処理、周辺機能、設定 |
| `src/ASF/` | Microchip ASFと同梱の第三者コンポーネント |
| `LoRa_Basic_SAMR35.ld` | リンカースクリプト |

ルートには本README、変更履歴、ライセンス文書があります。アップデーターとブートローダーは、このファームウェアのリポジトリとは別に扱います。

## ライセンスと無保証

Shachi-labが著作権を持つ自作部分は、**[MIT License](LICENSE.txt)**で提供します。著作権はShachi-labが保持します。

利用・改変・商用利用・配布・販売は自由です。著作権表示とライセンス全文（免責事項を含む）を保持してください。第三者コードには元のライセンスが適用されます。詳細は[THIRD_PARTY_NOTICES_jp.md](THIRD_PARTY_NOTICES_jp.md)を参照してください。

本ソフトウェアは無償・無保証で提供します。作者はMITライセンスに定めるとおり責任を負わず、サポートの提供義務も伴いません。配布者がサポートを提供するかどうかは、配布者の裁量です。使用・改変・製品への組み込みに際しては、利用者側で技適を含む適用法令・認証条件を確認してください。

## 有償サポート・開発のご相談

本ソフトウェアのサポートや、関連するファームウェアの開発は、**別途有償で承ります**。[Shachi-lab](https://shachi-lab.com/)へご相談ください。コードの利用許可に、作者によるサポートは含まれません。

本リポジトリのShachi-lab版に関するお問い合わせは、i2-electronicsへは行わないでください。ハードウェアに関するお問い合わせは、ハードウェアベンダーへお願いします。

---

Copyright (c) 2019–2026 Shachi-lab
