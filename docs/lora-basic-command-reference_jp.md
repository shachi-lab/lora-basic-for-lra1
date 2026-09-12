# LoRa-BASIC コマンド・変数・関数一覧

[English](lora-basic-command-reference.md)

この一覧は、LoRa-BASIC for LRA1のソースコードに登録されているコマンド、キーワード、変数、定数、関数をまとめた索引です。仕様、引数、設定範囲、動作条件はリンク先の各リファレンスマニュアルを参照してください。

名前の表記はList表示およびソースコードの登録文字列に合わせています。入力時は大文字と小文字を区別しません。ビルド設定、動作モード、設定レベルによって使用できない項目があります。

## BASIC文・コマンド・キーワード

プログラム制御、入出力、周辺機器、P2P通信および保守操作に使用する文、コマンド、構文キーワードです。

| 名前 | 書式 | 概要 | 詳細 |
|---|---|---|---|
| `Print` | `Print [式]`、`$Print [式]`、`%Print [式]`、`&Print [式]` または `?[式]` | 通常、16進数、パーセントエンコーディングまたはBase64形式で出力します。`?`は`Print`の省略形です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Input` | `Input [変数名]` | シリアルポートからの入力を待ち、入力された数値を変数名で指定した変数に格納します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Goto` | `Goto [ラベル]` | ラベルに指定された行にジャンプする。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Gosub` | `Gosub [ラベル]` | ラベルに指定された行のサブルーチンにジャンプします。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Return` | `Return` | サブルーチンから戻ります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `For` | `For [変数名=式１] To [式２] Step [式３] ～ Next` | 指定範囲でFor～Nextループを開始します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Next` | `Next` | Forループの次の反復へ進みます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Do` | `Do ～ Loop` または `Do ～ Loop While [式]` | 無条件、または末尾の条件が真の間繰り返すDo～Loopループを開始します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Loop` | `Loop` または `Loop While [式]` | Doループの末尾を示し、Whileを付けた場合は条件が真の間繰り返します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `While` | `While [式] ～ Loop` | 条件が真の間、ループを継続します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `If` | `If [式] Then ～ ElseIf [式] Then ～ Else ～ EndIf` | 条件分岐を開始します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Run` | `Run [ラベル]` | プログラムエリアのプログラムを実行します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Resume` | `Resume` | Stopコマンドまたは、シリアルポートからの「Breakキャラクタ/信号」で中断した処理を再開します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Stop` | `Stop` | プログラムの実行を中断します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `End` | `End` | プログラムの実行を終了して、REPLモードの入力待ちに戻ります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `New` | `New` | プログラムと変数をクリアします。 「プログラム入力禁止モード」ではエラーとなります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `List` | `List [[ラベル１] ，[ラベル２]]` | プログラムエリアのプログラムを[ラベル１]から[ラベル２]の範囲で行番号順に表示します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Renum` | `Renum [式]` | プログラムの行番号を付け替えます。 「プログラム入力禁止モード」ではエラーとなります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Edit` | `Edit [式]` | プログラム入力モードの禁止/許可を設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Delete` | `Delete [[ラベル１] ，[ラベル２]]` | プログラムを削除します。 「プログラム入力禁止モード」ではエラーとなります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Pload` | `Pload [！[ラベル]]` | FLASHメモリーに保存されたプログラムをプログラムエリアに読み込みます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Psave` | `Psave [＊] [式]` | プログラムエリアのプログラムをFLASHメモリーに保存します。 「プログラム入力禁止モード」ではエラーとなります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Ver` | `Ver` | ソフトウェアのバージョンを表示します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Delay` | `Delay time` | 指定した時間（1/1000秒単位）だけ動作を停止し、経過後に再開します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Pause` | `Pause` | シリアルポートから１文字が入力されるまで待ちます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Exit` | `Exit` | For～NextまたはDo/While～Loopを抜け、NextまたはLoop（Loop While）の直後へ移ります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Continue` | `Continue` | For～Next、Do/While～Loopのループ内での処理中に、本コマンド以降の処理をスキップして再び繰り返し処理を実行します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Randomize` | `Randomize [式]` | 乱数の種を設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Data` | `Data [式１], [式２]・・・` | Readコマンドで読み込む数値を記載します。（数値だけでなく、関数を含む演算式の記述も可能です。） | [基本編](lora-basic-manual_basics_jp.md) |
| `Read` | `Read [変数]` | Dataコマンドで記載された式の数値を、変数に読み込みます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Restore` | `Restore [ラベル]` | Readコマンドで読み込むDataコマンドの順番を変更します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Reset` | `Reset` | 本デバイスのシステムリセットを実行します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Sleep` | `Sleep [time] [,mode]` | 省電力モードで待機します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Deep` | `Deep [time] [,mode]` | 超省電力モード(Deep Sleep)で待機します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Outp` | `Outp [式１] , [式２]` | GPIOのポート状態を個別または一括で設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Swap` | `Swap [変数１]，[変数２]` | 変数１と変数２の内容を入れ替えます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Else` | `Else` | 条件が成立しなかった場合の処理を示します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `ElseIf` | `ElseIf expression Then` | 追加の分岐条件を指定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `EndIf` | `EndIf` | Ifブロックの末尾を示します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Lclr` | `Lclr` | LCDの表示をクリアします。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Lprint` | `Lprint [引数]` | LCDに文字列を表示します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `#?` | `#?` | 各種設定値を一括で表示します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Nop` | `Nop` | 何もせず次の処理へ進みます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Default` | `Default [式１]` | 各種設定をデフォルト値に戻します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Sload` | `Sload` | 設定値をFLASHメモリーから読み出します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Ssave` | `Ssave` | 設定値をFLASHメモリーに保存します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Send` | `Send [文字列]` | LoRaパケットを送信します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Recv` | `Recv [式１] [，式２]` | LoRaパケットの受信を開始します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `RxStop` | `RxStop` | 受信動作を終了します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Comm` | `Comm [式]` | 簡易的な双方向通信モードを開始します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Txif` | `Txif [文字列]` | 周期的にパケットを送信します。 | [保守編](lora-basic-manual-maintenance_jp.md) |
| `TxWave` | `TxWave [式]` | 連続して電波を送信します。 | [保守編](lora-basic-manual-maintenance_jp.md) |
| `Sens` | `Sens [式]` | 周期的(200msec)にRssiを計測して表示します。 | [保守編](lora-basic-manual-maintenance_jp.md) |
| `Bme` | `Bme [引数１[，引数２[，変数名３]]]` | 評価ボード上のBME280で「気温、湿度、気圧」を計測します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `I2cR` | `I2cR [式１] , [式２] , [式３]` | I2CデバイスからReadします | [基本編](lora-basic-manual_basics_jp.md) |
| `I2cW` | `I2cW [式１] , [式２] , [式３]` | I2CデバイスにＷｒｉｔｅします | [基本編](lora-basic-manual_basics_jp.md) |
| `Aload` | `Aload` | AsaveコマンドでFlashメモリーに保存した配列変数「@(0～255)」を読み込みます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Asave` | `Asave` | 全ての配列変数「@(0～255)」をFlashメモリーに保存します。Flashメモリーの内容は上書きされます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Utrans` | `Utrans [式]` | UART2を透過モードで使用します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Pwm` | `Pwm [式１] , [式２] , [式3]` | ポートにPwmを出力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Ugps` | `Ugps [式１], [式２]` | UART2に接続されたGPSデバイスから時刻、緯度、経度情報を取得します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Datetime` | `Datetime ( 式 )` | GPS制御のDatetimeコマンドを文字列中で使用すると、シリアル秒の文字列変換関数として機能します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Ext_cmd` | `Ext_cmd` | ビルド時に追加される拡張コマンドです。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Uprint` | `Uprint [式]` | UART2に[式]の結果を出力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Then` | `Then` | IfまたはElseIfの条件後に記述します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `To` | `To end-value` | Forループの終了値を指定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Step` | `Step increment` | Forループの増分を指定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Distance` | `Distance [式１]` | GPSで計測した2地点間の距離と角度を求めます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Catch` | `Catch [ラベル]` | エラー発生時の処理を定義します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Cad` | `Cad [式１] [，式２]` | プリアンブルの検出を待ちます。 （Cad = Channel Activity Detection） | [P2P編](lora-basic-manual_p2p_jp.md) |
| `MemDump` | `MemDump` | メモリー内容を表示する保守用コマンドです。 | [保守編](lora-basic-manual-maintenance_jp.md) |

## 文字列のエンコード表記

接頭辞を文字列または`Print`に付けることで、バイナリデータを目的の形式で入力・出力できます。

| 形式 | 文字列入力 | Print出力 |
|---|---|---|
| 通常 | `"..."` | `Print [式]` |
| 16進数 | `$"..."` | `$Print [式]` |
| パーセントエンコーディング | `%"..."` | `%Print [式]` |
| Base64 | `&"..."` | `&Print [式]` |

変換規則や使用例は[基本編](lora-basic-manual_basics_jp.md)を参照してください。

## 変数・定数

システム状態、設定値、通信パラメーターおよび送受信バッファを参照・設定する識別子です。項目によっては参照専用または設定専用です。

| 名前 | 書式 | 概要 | 詳細 |
|---|---|---|---|
| `@` | `@[index]` または `@(index)` | indexで要素を指定する数値配列です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `^` | `^` | LoRa-BASICで唯一の文字列変数です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Null` | `Null` | 数値として使用した場合は0、文字列として使用した場合は空文字列となる固定値です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Tick` | `Tick` | システム起動から1/1000秒毎にカウントアップされる値です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Clock` | `Clock` | システム起動から1秒毎にカウントアップされる値です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Inkey` | `Inkey、Inkey ( 式 )` | シリアルポート(UART)の入力バッファから取得した文字のASCIIコードです。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Auto` | `Auto` | システム起動時に自動実行するBASICコマンドを文字列で設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `False` | `False` | 偽を表す固定値0です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `True` | `True` | 真を表す固定値1です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Baud` | `Baud` | シリアルポートの転送速度（ボーレート）を設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Echo` | `Echo` | シリアルポートに入力された文字のエコーバックを設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Lpos` | `Lpos` | 評価ボード上のLCDに文字列を表示するときの表示位置を指定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Lcont` | `Lcont` | 評価ボード上のLCDのコントラストを設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Modem` | `Modem` | 変調方式を指定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Pwr` | `Pwr` | 送信出力を設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Sf` | `Sf` | 拡散率(Spread Factor)を設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Bw` | `Bw` | 帯域幅(Bandwidth)を設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Cr` | `Cr` | 符号化率(Cording Rate)を設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Ch` | `Ch` | 送受信チャンネルを設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Frq` | `Frq` | 送受信周波数を参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Gid` | `Gid` | グループIDを設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Own` | `Own` | 自局IDを設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Dst` | `Dst` | 宛先IDを設定します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Snr` | `Snr` | 受信したパケットのSnr値を参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Rssi` | `Rssi` | 受信パケットのRssi値の参照 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `DevId` | `DevId` | デバイス固有のEUI-64形式IDを参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Stat` | `Stat` | 送受信状態およびLoRaエラーの参照 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Period` | `Period` | Txifコマンドの送信間隔 | [保守編](lora-basic-manual-maintenance_jp.md) |
| `Ctrl` | `Ctrl` | 各種コントロールを設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Txd` | `Txd(式) または Txd` | 送信バッファの設定と参照 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Rxd` | `Rxd(式) または Rxd` | 受信バッファの参照 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Txdw` | `Txdw(offset)` | 送信バッファを16bit単位で設定・参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Rxdw` | `Rxdw(offset)` | 受信バッファを16bit単位で参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Txdl` | `Txdl(offset)` | 送信バッファを32bit単位で設定・参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Rxdl` | `Rxdl(offset)` | 受信バッファを32bit単位で参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Pass` | `Pass` | 設定レベルを設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `BrkCh` | `BrkCh` | プログラムの動作を中断するために使用する「Breakキャラクタ」のASCIIコードとBreak信号を設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Level` | `Level` | キャリアセンスレベル | [保守編](lora-basic-manual-maintenance_jp.md) |
| `I2cD` | `I2cD(式) または I2cD` | I2Cバッファの設定と参照 | [基本編](lora-basic-manual_basics_jp.md) |
| `Sn` | `Sn` | シリアルナンバー | [保守編](lora-basic-manual-maintenance_jp.md) |
| `Drift` | `Drift` | 設定周波数の偏差 | [保守編](lora-basic-manual-maintenance_jp.md) |
| `MpuId` | `MpuId` | MCU固有IDを参照します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Xtal32k` | `Xtal32k` | 低速クロック(32.768kHz)を以下の種類に設定します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Ubaud` | `Ubaud` | UART2のボーレートを設定、参照します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Uinkey` | `Uinkey または Uinkey（式）` | UART2から１文字分を入力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `AesKey` | `AesKey` | 暗号化用の共通鍵を設定、参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Error` | `Error` | エラーコードの参照またはエラーをスローします。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Gets` | `Gets、Gets ( 式 )` | シリアルポート(UART)の入力バッファから１行分の文字列を入力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Ugets` | `Ugets または Ugets（式）` | UART2から文字列を入力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `TxLen` | `TxLen` | 送信可能バイト数の参照 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Hop` | `Hop` | P2P転送機能で使用するパケットの最大転送回数を設定・参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `FwdConf` | `FwdConf` | P2P転送時に使用する送信パラメーター（Sf、Bw、Cr、Ch）を設定・参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Cause` | `Cause` | システムの起動原因を表す以下の32ビット値です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `SpiBaud` | `SpiBaud` | SPIのボーレートを設定して、GPIOポートでのSPI機能を有効にします。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Bup1` | `Bup1` | リセット時にバックアップされるBup1とBup2の独立した２つの変数です。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Bup2` | `Bup2` | バックアップ領域の値を設定・参照します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Milisec` | `Milisec` | 直前のClock変数を参照したときのミリ秒（0～999）を取得します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Txdm` | `Txdm(offset)` | 送信バッファを24bit単位で設定・参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Rxdm` | `Rxdm(offset)` | 受信バッファを24bit単位で参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `ProtId` | `ProtId` | P2PパケットのプロトコルIDを設定・参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `SyncWord` | `SyncWord` | P2P通信のSync Wordを設定・参照します。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Debug` | `Debug` | デバッグ機能のビット設定を保持します。 | [保守編](lora-basic-manual-maintenance_jp.md) |

## 関数

式の中で値の計算、変換、入出力およびバッファ処理を行い、結果を返します。

| 名前 | 書式 | 概要 | 詳細 |
|---|---|---|---|
| `Rnd` | `Rnd ( 式 )` | 式が０のときは、０～2,147,483,647 ($7FFF FFFF) の乱数が戻ります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Abs` | `Abs ( 式 )` | 式の絶対値が戻ります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Chr` | `Chr ( 式 )` | 式の下位8ビットをそのまま文字列中に出力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `WChr` | `WChr ( 式 )` | 式の下位１６ビット(Little endian)をそのまま文字列中に出力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Form` | `Form(書式文字列、式)` | D ：10進数（デフォルト） | [基本編](lora-basic-manual_basics_jp.md) |
| `Inp` | `Inp ( 式 )` | GPIOのポートの状態を取得します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Adc` | `Adc ( 式 )` | AdcポートからAD変換された値を取得します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Int8` | `Int8 ( 式 )` | 式の下位８ビットだけを変換対象とし、それより上位のビットは０とみなします。下位８ビットを符号付８ビット値として解釈した値を返します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Int16` | `Int16 ( 式 )` | 式の下位１６ビットだけを変換対象とし、それより上位のビットは０とみなします。下位１６ビットを符号付１６ビット値として解釈した値を返します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `IsFree` | `IsFree ( 式 )` | Chの空き状況をセンスします。 | [P2P編](lora-basic-manual_p2p_jp.md) |
| `Atan2` | `Atan2(式１、式２)` | 式２／式１の逆正接が戻ります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Sqrt` | `Sqrt ( 式 )` | 式１の平方根が戻ります。 式１が負数のときは2,147,483,647 ($7FFF FFFF) | [基本編](lora-basic-manual_basics_jp.md) |
| `Pow` | `Pow ( 式１、式２ )` | 式１の値を、式２の乗数で累乗した値が戻ります。 式２が０以下のときは1が戻ります。 | [基本編](lora-basic-manual_basics_jp.md) |
| `SpiTRx` | `SpiTRx (式１[ ，式２] )` | 式１の値をSPIポートに出力し、SPIポートから入力した値を返します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Bswap16` | `Bswap16 ( 式 )` | ３２ビットの値を、１６ビット毎に上位８ビットと下位８ビットを入れ替えます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Bswap32` | `Bswap32 ( 式 )` | ３２ビットの値を、８ビット単位で上位と下位を入れ替えます。 | [基本編](lora-basic-manual_basics_jp.md) |
| `TimeVal` | `TimeVal( 式 )` | 配列変数の日付(年、月、日)と時刻（時、分、秒）をシリアル秒に変換します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `LChr` | `LChr ( 式 )` | 式の下位３２ビット(Little endian)をそのまま文字列中に出力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `MChr` | `MChr ( 式 )` | 式の下位２４ビット(Little endian)をそのまま文字列中に出力します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Int24` | `Int24 ( 式 )` | 式の下位２４ビットだけを変換対象とし、それより上位のビットは０とみなします。下位２４ビットを符号付２４ビット値として解釈した値を返します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Bcc` | `Bcc ( 文字列 )` | 文字列の水平パリティを取得します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `Sum` | `Sum ( 文字列 )` | 文字列の総和を取得します。 | [基本編](lora-basic-manual_basics_jp.md) |
| `ToInt` | `ToInt( 文字列 [、[オフセット] [、[文字数]]] )` | 文字列中の指定部分を数値に変換します。 | [基本編](lora-basic-manual_basics_jp.md) |

## LoRaWANコマンド

LoRaWANへの参加、送受信、設定保存およびチャンネル操作を実行します。`Wan_`は`#`で省略できます。

| 名前 | 書式 | 概要 | 詳細 |
|---|---|---|---|
| `Wan_Join` | `Wan_Join` | アクティベーション(Join)を実行 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_UnJoin` | `Wan_UnJoin` | アクティベーション(Join)を解除 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_LinkChk` | `Wan_LinkChk` | リンクチェックMACコマンドを設定 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Tx` | `Wan_Tx [encode]` | アップリンクデータを送信 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Rx` | `Wan_Rx [timeout , ] [encode]` | ダウンリンクデーターを受信 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Save` | `Wan_Save` | 設定値とJoin状態を保存 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_ChList` | `Wan_ChList` | チャンネル一覧を表示 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_RxStop` | `Wan_RxStop` | ダウンリンクデーターの受信を停止 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_ChAdd` | `Wan_ChAdd chid, Freq, dr-min, dr-max` | 使用チャンネルをチャンネルリストに追加 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_ChRemove` | `Wan_ChRemove chid` | 使用チャンネルをチャンネルリストから削除 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |

## LoRaWAN変数

LoRaWANの設定、鍵、カウンターおよび通信状態を参照・設定します。設定可否や保存方法は項目ごとに異なります。

| 名前 | 書式 | 概要 | 詳細 |
|---|---|---|---|
| `Wan_Mode` | `Wan_Mode` | LoRa-BASICの通信モードを設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Class` | `Wan_Class` | LoRaWANのクラスを設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_TxPwr` | `Wan_TxPwr` | 送信出力を設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_FPort` | `Wan_FPort` | 送信ポート番号を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Cnf` | `Wan_Cnf` | 送達確認を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Adr` | `Wan_Adr` | ADR (Adaptive Data Rate) を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Dr` | `Wan_Dr` | データーレート(Data Rate)を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Trial` | `Wan_Trial` | 送信試行回数を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_DevEui` | `Wan_DevEui` | DevEui (Device EUI) を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_JoinEui` | `Wan_JoinEui` | JoinEui (AppEui) を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_AppKey` | `Wan_AppKey` | AppKey (Application Key) を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_DevAddr` | `Wan_DevAddr` | DevAddr (Device Address) を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_NetID` | `Wan_NetID` | LoRaWANのNetIDを設定・参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_AppSkey` | `Wan_AppSkey` | AppSkey (Application Session Key) を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_NwkSkey` | `Wan_NwkSkey` | NwkSkey (Network Session Key) を参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_JoinStat` | `Wan_JoinStat` | アクティベーションの状態を参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_UpCnt` | `Wan_UpCnt` | Uplinkカウンタを設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_DownCnt` | `Wan_DownCnt` | Downlinkカウンタを設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Length` | `Wan_Length` | 現在のデーターレートで送信可能な送信バイト数を参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_JoinMode` | `Wan_JoinMode` | アクティベーション方式を設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_ChMask` | `Wan_ChMask` | チャンネルマスクを参照/設定します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_Pending` | `Wan_Pending` | ダウンリンクデーターの保留状態を参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_RecvDelay1` | `Wan_RecvDelay1` | RECEIVE_DELAY1 を設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_RecvDelay2` | `Wan_RecvDelay2` | RECEIVE_DELAY2 を設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_JoinDelay1` | `Wan_JoinDelay1` | JOIN_ACCEPT_DELAY1 を設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_JoinDelay2` | `Wan_JoinDelay2` | JOIN_ACCEPT_DELAY2 を設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_TimeReq` | `Wan_TimeReq` | DEVICE_TIME_REQのMACコマンドを設定/参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_DutyTime` | `Wan_DutyTime` | 送信制限時間を参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_DwellStat` | `Wan_DwellStat` | Dwellの状態を参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_LastTxCh` | `Wan_LastTxCh` | 最後に送信に使用したチャンネルを参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_MacCmd` | `Wan_MacCmd` | 保持されているLoRaWAN MACコマンドを16進数文字列で参照します。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |
| `Wan_BatLevel` | `Wan_BatLevel` | LRA1からLoRaWANネットワークへ報告するバッテリーレベルを設定・参照します。サーバーから取得する値ではありません。 | [LoRaWAN編](lora-basic-manual-wan_jp.md) |

## 別名と省略記法

入力を短くするために使用できる省略形と、旧バージョンとの互換性のために残されている名称です。

| 表記 | 意味 |
|---|---|
| `?` | `Print`の省略形 |
| `#Join`、`#Tx`など | `Wan_Join`、`Wan_Tx`など、`Wan_`接頭辞の省略形 |
| `Wan_AppEui` | Firmware Ver1.07.f以前で使用されていた`Wan_JoinEui`の旧名称 |

`#?`はLoRa送受信バッファを操作する独立したコマンドで、`Print`の省略形ではありません。
