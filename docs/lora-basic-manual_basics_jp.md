<h1>
LoRa-BASIC for LRA1<br/>
リファレンスマニュアル<br/>
基本編
</h1>

コマンド名、関数名、システム変数名は大文字と小文字を区別しません。本書では、List表示およびソースコードの登録表記に合わせて記載します。

#  概要

LRA1モジュールにはBASIC言語の機能を簡略化（サブセット化）したLoRa-BASICを搭載しています。  

LoRa-BASICはnanoBASICから派生し、LoRa無線やデバイス固有の機能を制御できるよう拡張されています。  

LoRa-BASICの文法やコマンド等は一般的なBASIC言語に類似していますが、仕様が異なる部分があります。

本マニュアルでは、lora-BASICの文法や基本コマンドおよびデバイス特有の機能について記載します。  

LoRa-P2Pモードに関しては、***「LoRa-BASIC for LRA1 リファレンスマニュアル LoRa-P2P編」*** を参照してください。
LoRaWANモードに関しては、***「LoRa-BASIC for LRA1 リファレンスマニュアル LoRaWAN編」*** を参照してください。

なお、本マニュアルでは、LRA1モジュールを「デバイス」と表記します。

>地域・無線認証に関する注意  
>本マニュアルに記載する無線機能は、日本国内での使用を想定しています。日本国外での使用および各国・地域の無線規制・認証への適合についてはサポートしていません。  


---

# 接続

## PCとの接続

本デバイスのUARTとPCをシリアルポート経由で接続し、TeraTerm等のシリアル通信ソフトからテキストベースで制御します。（デバイスのUARTとPCとの接続にはUART-USB変換器等の機器が必要な場合があります。）  
デフォルトのシリアルポート通信速度は115,200bps（パリティ無し）です。  
本マニュアルでは、TeraTermを使用した場合で解説します。  
TeraTermは以下の設定を推奨します。

<table style="width:81%;">
<colgroup>
<col style="width: 29%" />
<col style="width: 22%" />
<col style="width: 29%" />
</colgroup>
<thead>
<tr>
<th>設定ダイアログ</th>
<th>設定項目</th>
<th>設定値</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="7">シリアルポートの設定と接続</td>
<td>ポート</td>
<td>PCのデバイス設定による</td>
</tr>
<tr>
<td>スピード</td>
<td>115200</td>
</tr>
<tr>
<td>データ</td>
<td>8 bit</td>
</tr>
<tr>
<td>パリティ</td>
<td>none</td>
</tr>
<tr>
<td>ストップビット</td>
<td>1 bit</td>
</tr>
<tr>
<td>フロー制御</td>
<td>none</td>
</tr>
<tr>
<td>送信遅延</td>
<td>0ミリ秒/字、10ミリ秒/行</td>
</tr>
<tr>
<td rowspan="4">端末の設定</td>
<td>改行コード</td>
<td>受信 CR、送信 CR</td>
</tr>
<tr>
<td>ローカルエコー</td>
<td>チェックなし</td>
</tr>
<tr>
<td>漢字-受信</td>
<td>UTF-8</td>
</tr>
<tr>
<td>漢字-送信</td>
<td>UTF-8</td>
</tr>
</tbody>
</table>

## 外部マイコンとの接続

外部のMCU等から本デバイスを使用する場合も、PCのTeraTermからと同様にシリアルポートで制御することになります。単にLoRa送受信だけを行う場合は、起動時に「Recv」や「Comm」コマンドを実行するように設定することで、外部マイコンとは送受信データのみのやり取りにすることも可能です。（詳細は各コマンドの解説を参照）

## 外部ペリフェラルとの接続

LoRa-BASICには評価ボードに搭載されている外部ペリフェラル（BME280、LCD）を制御する機能を実装しています。

他のペリフェラルの使用は想定されていません。

## 外部ピンの制御機能

LoRa-BASICはデバイスの外部ピンをコマンド等から制御することができます。  
各ピンのコマンドによる制御機能は下表のようになります。

<table style="width:85%;">
<colgroup>
<col style="width: 6%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 14%" />
</colgroup>
<thead>
<tr>
<th rowspan="2" style="text-align: center;">ピン番号</th>
<th rowspan="2" style="text-align: center;">名称</th>
<th colspan="6" style="text-align: center;">コマンドによる機能</th>
<th rowspan="2" style="text-align: center;">SYSTEM</th>
</tr>
<tr>
<th style="text-align: center;">GPIO</th>
<th style="text-align: center;">Adc</th>
<th style="text-align: center;">Pwm</th>
<th style="text-align: center;">UART2</th>
<th style="text-align: center;">I2C</th>
<th style="text-align: center;">SPI</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">PB02</td>
<td style="text-align: center;">34</td>
<td style="text-align: center;">10</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">MISO</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">2</td>
<td style="text-align: center;">PA05</td>
<td style="text-align: center;">5</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">UART-RX</td>
</tr>
<tr>
<td style="text-align: center;">3</td>
<td style="text-align: center;">PA04</td>
<td style="text-align: center;">4</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">UART-TX</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td style="text-align: center;">PB03</td>
<td style="text-align: center;">35</td>
<td style="text-align: center;">11</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td style="text-align: center;">PA27</td>
<td style="text-align: center;">27</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">PB22</td>
<td style="text-align: center;">54</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">MOSI</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">PA06</td>
<td style="text-align: center;">6</td>
<td style="text-align: center;">6</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">WAKEUP</td>
</tr>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">PA07</td>
<td style="text-align: center;">7</td>
<td style="text-align: center;">7</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">8</td>
<td style="text-align: center;">16</td>
<td style="text-align: center;">8 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">10</td>
<td rowspan="2" style="text-align: center;">VDD</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">11</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">12</td>
<td rowspan="2" style="text-align: center;">GND</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">13</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">14</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">9</td>
<td style="text-align: center;">17</td>
<td style="text-align: center;">9 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">15</td>
<td style="text-align: center;">PA13</td>
<td style="text-align: center;">13</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">PA16</td>
<td style="text-align: center;">16</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">SDA</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">PA17</td>
<td style="text-align: center;">17</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">SCL</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">18</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">18 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">LED-TX</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">PA28</td>
<td style="text-align: center;">28</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">20</td>
<td style="text-align: center;">PA23</td>
<td style="text-align: center;">23</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">RX</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">21</td>
<td style="text-align: center;">PB23</td>
<td style="text-align: center;">55</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">SCLK</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">GND</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">23</td>
<td style="text-align: center;">PA15</td>
<td style="text-align: center;">15</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">PA14</td>
<td style="text-align: center;">14</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">ACTIVE</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">19</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">19 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">LED-RX</td>
</tr>
<tr>
<td style="text-align: center;">26</td>
<td style="text-align: center;">PA22</td>
<td style="text-align: center;">22</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">TX</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">25</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">25 [1]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">24</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">24 [1]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">29</td>
<td style="text-align: center;">PA31</td>
<td style="text-align: center;">31</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">30</td>
<td style="text-align: center;">PA30</td>
<td style="text-align: center;">30</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">31</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">1 [2]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">XOUT32</td>
</tr>
<tr>
<td style="text-align: center;">32</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">0 [2]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">XIN32</td>
</tr>
<tr>
<td style="text-align: center;">33</td>
<td style="text-align: center;">Reset</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">34</td>
<td style="text-align: center;">GND</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
</tbody>
</table>

各機能の数値はコマンドおよび関数で指定する引数です。（Pwmの\[n\]はグループを表します。）  
各コマンド等の詳細は、それぞれのコマンドの説明を参照してください。

# ユーザーインタフェース

## 文字列行の入力

コマンド等の文字列が入力可能な状態になると、コマンドプロンプト「＞」が表示されます。

シリアルポートから入力された文字列はCr（改行）までを１行とします。

コマンド文および変数名は大文字/小文字を区別しません。

コメントおよび文字列中の内容は大文字小文字が区別され、入力したとおりとなります。

１行の文字数は254文字以内です。254文字を超えた入力は無視されます。

### 編集入力

行の入力では以下の編集操作が可能です。（TeraTermをデフォルトのキー設定で使用した場合）

<table style="width:93%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 65%" />
<col style="width: 13%" />
</colgroup>
<thead>
<tr>
<th>入力</th>
<th>動作</th>
<th>コード</th>
</tr>
</thead>
<tbody>
<tr>
<td>文字入力</td>
<td>カーソルが行末にあるときは行末に文字を追加<br />
カーソルが行途中にあるときはカーソル直前に文字を挿入</td>
<td>$20～<br />
UTF-8</td>
</tr>
<tr>
<td>「Backspace」</td>
<td>カーソルの直前にある文字を削除</td>
<td>$08</td>
</tr>
<tr>
<td>「Delete」</td>
<td>カーソルの位置にある文字を削除</td>
<td>$7f</td>
</tr>
<tr>
<td>「Tab」</td>
<td>スペースと同じ</td>
<td>$09</td>
</tr>
<tr>
<td>「←」</td>
<td>カーソルを左へ移動</td>
<td>$1b,[,D</td>
</tr>
<tr>
<td>「→」</td>
<td>カーソルを右に移動</td>
<td>$1b,[,C</td>
</tr>
<tr>
<td>「↑」</td>
<td>前の履歴へ戻る</td>
<td>$1b,[,A</td>
</tr>
<tr>
<td>「↓」</td>
<td>次の履歴へ進む。なければListコマンドで最後に表示したプログラム行</td>
<td>$1b,[,B</td>
</tr>
<tr>
<td>「Home」</td>
<td>カーソルを行頭に移動</td>
<td>$1b,[,~,1</td>
</tr>
<tr>
<td>「End」</td>
<td>カーソルを行末に移動</td>
<td>$1b,[,~,4</td>
</tr>
<tr>
<td>「Enter」</td>
<td>入力完了（CR）</td>
<td>$0d</td>
</tr>
<tr>
<td>Ctrl-C</td>
<td>入力中断 （設定で文字コードの変更が可能）</td>
<td>$03</td>
</tr>
</tbody>
</table>

> *注意事項*  
> ・文字コードの\$1F以下は文字入力できません。  
> ・入力中断と改行のみの入力は入力履歴にはなりません。  
> ・履歴は最大10件および履歴全体で256文字以内です。  
> ・複数行にわたってのカーソル移動ができません。
> （入力が複数行になると編集が正常にできません）  
> ・ターミナルの種類を"VT100"に設定してください。
> （TeraTermでは、\[設定\]-\>\[端末\]-\>\[端末ID\]）  
> ・漢字コードは"UTF-8"です。
> （TeraTermでは、\[設定\]-\>\[端末\]-\>\[漢字-受信/送信\]）  
> ・履歴を表示すると入力中の内容は削除され復旧しません。

## インタプリタ

LoRa-BASICはインタプリタ方式です。入力された文字列は中間言語のバイトコードに変換し、各コマンドの処理を順次実行します。または、中間言語に変換されたバイトコードをプログラム領域に格納します。

一般的に、入力されたコマンド文字列よりも中間言語への変換後のバイトコードの方がバイト数は少なくなります。

バイトコードを格納するプログラム領域のサイズは**8192バイト**です。このプログラム領域はRAM上にあるため、システムリセットや電源断によって消去されますが、内部FLASHメモリーへの書き込み/読み出しが可能です。

## 通信モード

通信モードには、「REPLモード」と「RUNモード」があります。

***REPLモード***

シリアルポートからの入力文字列をコマンド文として、１行毎に対話形式でコマンドを実行します。

***RUNモード***

RAM上のプログラムエリアにあるBASICプログラムを順次実行します。

本デバイスの起動直後は「REPLモード」です。

Auto変数の設定によって、起動直後に指定のコマンドを自動実行することが可能です。

## プロテクトモード

BASICプログラムを外部に表示させないように、「プロテクトモード」に設定することができます。  
プロテクトモードでは、Edit/List/Psaveコマンドの実行が制限されます  
以下の操作でプロテクトモードに設定できます。  
　(1) Pass変数に-1を設定する。  
　(2) Psave \* コマンドでFlashに保存する。  
　(3) プロテクト指定のPassキーが設定されたBASE64コードをPload＆で読み込む。  

>※注意事項  
>(1)と(2)は、リセット後もプロテクトモードを有効にするにはSsaveでの保存が必要です。  
>(3)の場合に限りPsaveを実行すると同時にSsaveも実行されます。

Ssaveされたプロテクトモードを解除するには ”Default -1”コマンドを使用します。  
詳細は、上記の各変数及びコマンドの説明を参照してください。

# 言語仕様

## 変数

### 変数の種類

一般変数 ：
A～Zのアルファベット１文字（26個）で表わします。（大文字小文字の違いはありません）

配列変数 ： @(添字)で表される１次元配列が１つ。（添字の範囲は0～255）  
　　　　　　　　添字には式を使用できます。 丸括弧”(添字)”
の代わりに角括弧”\[添字\]”を使用可能です。

特殊変数 ： システム変数やLoRa変数など、特別な用途の変数
（詳細は各変数の項目を参照してください）

文字列変数： 「　^　(キャレット) 」、「　.　（ドット）」  
一般変数と配列変数は、32ビット符号付整数です。  
すべての変数は大域（グローバル）変数で、局所（ローカル）変数はありません。

### 変数の初期化

リセット（電源ONリセット、ハードウェアリセット、電圧低下リセット、ウォッチドッグリセット、Deepコマンドからの復帰、Resetコマンド）で、いくつかの特殊変数はFlashに保存された値に再設定されます。  
上記リセットまたはNewおよびRunコマンド実行時には、一般変数と配列変数は０に、文字列変数は文字列長０の空文字列に初期化されます。（Bup1、Bup2変数は電源ONリセット時のみ初期化されます。）

## 数値リテラル

使用可能な数値リテラルは符号付き32ビット整数のみです。

数値リテラルに「-（マイナス符号）」は使用できますが、符号としての「＋（プラス符号）」は使用できません。

デフォルトの数値は１０進数で、数値の前に「＄」をつけると１６進数となります。

## 文字列リテラル

文字列リテラルはダブルクォーテーションで囲みます。
文字コードにはUTF-8が使用できます。

Print文などの一部のコマンドや、文字列式での使用ができます。

ダブルクォーテーションの前にデコード記号（％、＆、＄）を付けると文字列がデコードされます。

## 式と演算

### 式

数値定数および以下の演算子を使用したものが「式」となります。

演算中のオーバーフローはエラーになりませんが、０で除算した場合は"Division by 0"のエラーになります。  
若干の制限はありますが文字列と数値の混在も可能です。（式の中の文字列以降は文字列式とみなされます。）

### 算術演算子

以下の算術演算子が使用できます。

| 演算子 | 演算 | 記述例                                                        |
|:------:|:----:|---------------------------------------------------------------|
|   ＋   | 加算 | A=B+C                                                         |
|   －   | 減算 | A=B-C                                                         |
|   ＊   | 乗算 | A=B\*C                                                        |
|  ＊＊  | 累乗 | A=B\*\*C ※Pow(B,C)関数のシンタックスシュガー                  |
|   ／   | 除算 | A=B/C ※C99準拠 （小数点以下はゼロ方向へ切り捨て）             |
|  ／／  | 対数 | A=B//C ※log <sub>C</sub> B = log B/log C （簡易演算の概算値） |
|   ％   | 剰余 | A=B%C ※C99準拠（演算結果は被除数の符号と一致）                |

### 単項演算子

以下の単項演算子が使用できます。

| 演算子 |      演算       | 記述例         |
|:------:|:---------------:|----------------|
|   －   |    符号反転     | A=-B, A=-(B+C) |
|   ～   |   ビット反転    | A=~B, A=~(B+C) |
|   ！   | 論理否定（NOT） | A=!B, A=!(B+C) |

以下の単項演算子は一般変数と配列変数およびCtrl、Bup1、２変数の直後にのみ単独で使用可能です。  
（文字列変数、LoRa変数、システム変数、および式の中には記述できません。）

| 演算子 |      演算      | 記述例 | 意味  |
|:------:|:--------------:|--------|-------|
|  ＋＋  | インクリメント | A++    | A=A+1 |
|  －－  |  デクリメント  | A--    | A=A-1 |

### 論理演算子

以下の論理演算子が使用できます。

| 演算子 |     演算     | 記述例                        |
|:------:|:------------:|-------------------------------|
|  ＆＆  | 論理積 (AND) | If A=B && A=C Then C=A&&B     |
|  ｜｜  | 論理和 (OR)  | If A=B \|\| A=C Then C=A\|\|C |

### ビット演算子

以下のビット演算子が使用できます。

| 演算子 | 演算 | 記述例 |
|:--:|:--:|----|
| ＆ | 論理積(AND) | A=B&C |
| ｜ | 論理和(OR) | A=B\|C |
| ^ | 排他的論理和(XOR) | A=B^C 　　　　　　　　　　※「^（キャレット）」文字列とは区別されます |

### 比較演算子

以下の比較演算子が使用できます。

|  演算子  |  演算  | 記述例                                                     |
|:--------:|:------:|------------------------------------------------------------|
|  =、= =  |  一致  | If A=B Then A=B==C 　※「＝（代入演算子）」とは区別されます |
| \<\>、!= | 不一致 | If A\<\>B Then A=B!=C                                      |
|    \<    |  未満  | If A\<B Then A=B\<C                                        |
|   \<=    |  以下  | If A\<=B Then A=B\<=C                                      |
|    \>    |  超過  | If A\>C Then A=B\>C                                        |
|   \>=    |  以上  | If A\>=B Then A=B\>=C                                      |

比較結果が真のときは１、偽のときは０となります。

### シフト演算子

以下のシフト演算子が使用できます。

| 演算子 | 演算 | 記述例 |
|:--:|:--:|----|
| \>\> | 算術右シフト | A=B\>\>C　　　　　　　　　　※C99準拠（MSBの符号ビットはそのまま） |
| \<\< | 論理左シフト | A=B\<\<C　　　　　　　　　　※C99準拠（MSBの符号ビットもシフト） |

### 括弧

( ) : 括弧  
カッコ内の演算が優先されます。（文字列→数値変換とは区別されます）  
例 : A=((B+C)/D)\>(E+1)

### 文字列式

文字列や文字列変数を連続して記述するか「；（セミコロン）」で接続すると、１つのつながった文字列となります。Printコマンドでは、文字列を「，（カンマ）」で接続すると「TAB
(ASCIIコード=0x09)」で接続されます。（なお、Print、Lprintコマンドの引数文字列の最後がセミコロンの場合は改行しません。）

文字列式には括弧や数値演算子は使用できません。（括弧で囲むと数値扱いになります。）

### 演算子の優先度

式における演算の優先順位は以下の様になります。

括弧 ＞ 単項演算子 ＞ 乗算・除算・剰余・累乗・対数 ＞ 加算・減算 ＞
比較演算子・シフト演算子  
＞ ビット演算子・論理演算子　（同一優先度では、左→右
の順に演算します。）

## 代入文

「変数 =
式」のように、変数名に続けて代入演算子「＝」の後に式を記述します。

式の演算結果を変数に代入します。（一般的なBASICにあるような
LETコマンドはありません。）  
変数には一般変数（A～Z）、配列変数（＠）、LoRa変数、システム変数、文字列変数を使用できます。  
LoRa変数、システム変数、文字列には代入演算子の「単純代入“＝”」のみが使用可能で、一般変数と配列変数およびCtrl、Bup1、２変数には以下のような算術演算子と組み合わせた複合代入演算子による記述が可能です。

<table style="width:89%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 19%" />
<col style="width: 28%" />
<col style="width: 28%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">演算子</th>
<th style="text-align: center;">演算</th>
<th>記述例</th>
<th>意味</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">＝</td>
<td style="text-align: center;">単純代入</td>
<td colspan="2">A=B 　
　　　　　　　　　　　※代入可能なすべての変数に使用可能</td>
</tr>
<tr>
<td style="text-align: center;">＋＝</td>
<td style="text-align: center;">加算代入</td>
<td>A+=B</td>
<td>A=A+B</td>
</tr>
<tr>
<td style="text-align: center;">－＝</td>
<td style="text-align: center;">減算代入</td>
<td>A-=B</td>
<td>A=A-B</td>
</tr>
<tr>
<td style="text-align: center;">＊＝</td>
<td style="text-align: center;">乗算代入</td>
<td>A*=B</td>
<td>A=A*B</td>
</tr>
<tr>
<td style="text-align: center;">／＝</td>
<td style="text-align: center;">除算代入</td>
<td>A/=B</td>
<td>A=A/B</td>
</tr>
<tr>
<td style="text-align: center;">％＝</td>
<td style="text-align: center;">剰余代入</td>
<td>A%=B</td>
<td>A=A%B</td>
</tr>
<tr>
<td style="text-align: center;">｜＝</td>
<td style="text-align: center;">論理和代入</td>
<td>A|=B</td>
<td>A=A|B</td>
</tr>
<tr>
<td style="text-align: center;">＆＝</td>
<td style="text-align: center;">論理積代入</td>
<td>A&amp;=B</td>
<td>A=A&amp;B</td>
</tr>
<tr>
<td style="text-align: center;">＾＝</td>
<td style="text-align: center;">排他的論理和代入</td>
<td>A^=B</td>
<td>A=A^B</td>
</tr>
<tr>
<td style="text-align: center;">&gt;&gt;＝</td>
<td style="text-align: center;">算術右シフト代入</td>
<td>A&gt;&gt;=B</td>
<td>A=A&gt;&gt;B</td>
</tr>
<tr>
<td style="text-align: center;">&lt;&lt;＝</td>
<td style="text-align: center;">論理左シフト代入</td>
<td>A&lt;&lt;=B</td>
<td>A=A&lt;&lt;B</td>
</tr>
</tbody>
</table>

単純代入に限り左辺を省略可能で、式の演算後にその演算結果は破棄されます。

## 文字列と数値の相互変換

文字列と数値はお互いに変換が可能です。

文字列から数値への変換または文字列から数値への変換は、式の内容に応じて自動的に行われます。

### 数値→文字列

#### １0進数文字列への変換

代入文の代入先が文字列変数の場合や、Printコマンドのようにコマンドの引数が文字列の場合は、数値式の演算結果が１０進数の文字列に自動的に変換されます。  
例 ：
```
 Print 123+123 --> 246  
 ^="@"(1234+\$1234)/10"#" --> ^="@589#"  
```
桁数を指定する場合はForm関数の代わりに、0（式１,式２）
のような記述ができます。この場合、式１の数値を式２で指定した桁数の１０進数文字列に変換します。式２は省略可能で、式２を０または省略したときは、その数値を表す最小桁の１０進数文字列に変換されます。（式２が正数は空白で、負数はゼロで先行して桁数を合わせます。）  
詳細な指定するときはForm関数を使用してください。（詳細は「[8.3.5](#form)
[Form](#form)」を参照してください。）  
例 ：
```
 Print 0(123+123) --> 246　:　「Print 123+123 と等価」  
 ^="@"0(5678+123,6)"#"  --> ^="@ 5801#"  
 ^="@"0(5678+123,-6)"#" --> ^="@005801#"
```

#### １６進数文字列への変換

Form関数での１６進数文字列変換の代わりに、\$（式１,式２）
のような記述ができます。この場合、式１の数値を式２で指定した桁数の１６進数文字列に変換します。式２は省略可能で、式２を０または省略したときは、その数値を表す最小桁の１６進数文字列に変換されます。１６進数の’A～’F’は英大文字に変換されるので、英小文字に変換するなど詳細な指定にはForm関数を使用してください。（式２が正数は空白で、負数はゼロで先行して桁数を合わせます。）  
例 ：
```
 Print \$(123+123) --> F6　:　「Print Form("X",123+123) と等価」  
 ^="@"\$(5678+123,6)"#"  --> ^="@ 16A9#"  
 ^="@"\$(5678+123,-6)"#" --> ^="@0016A9#"
```

### 文字列→数値

文字列の代入先が文字列以外の変数やコマンドの引数のとき、および文字列式を括弧で囲むと、文字列を１０進数の数値として自動的に変換します。このとき文字列の内容が＄から始まるときは１６進数となります。数値以外の文字列は０となります。文字列の途中に数値以外の文字がある場合はその直前までの数値が変換対象です。  
式の最後が文字列式の場合は括弧で囲まなくてもエラーになりません。

例 ：
```
 A=("12";"3";"456")\*100  --> A=1235600  
 B=\$123+"\$Beef0000"     --> B=-1091632861  
```

ToInt関数を使用すると、文字列中の任意の位置から文字列を数値に変換できます。

## マルチステートメント文

コマンドは、「：」（コロン）でつなげてマルチステートメントが可能です。

ただし、１行は254文字または中間コードに変換したときに254byte以下で、それを超えるとエラーとなります。

## 行番号 (Line number)

プログラム各行の先頭につけられた番号です。すべてのプログラム行には行番号が必要です。  
行番号は０～65535の数値です。

プログラムは行番号の順番に実行されます。

プログラムを入力したときは、行番号順に並び変えられます。

## ラベル

プログラムの
GotoやGosubなどの分岐先には行番号の他にラベル文字列を使うことができます。

LoRa-BASICでは「＿（アンダーバー）」から始まる文字列をラベルとして扱います。

ラベルは行番号直後のみ有効です。行途中のラベルはラベルとしては無効ですがエラーにはなりません。

プログラム中に同一のラベル文字列が存在した場合、プログラムの先頭に近いほうが有効とります。

行番号はすべてのプログラム行に必要ですが、ラベルは分岐先などで必要に応じて付けてください。  
ラベル文字列に使用できる文字は、「英大文字、英小文字、数字、アンダーバー、ドット」で、英大文字/小文字は区別されます。文字列ラベルは「：（コロン）」または行末で終端してください。  
ラベルを数式中で使用すると、そのラベルの行番号が参照されます。（存在しないラベル名の場合は０が戻ります）

## コメント

「’（シングルクォート）」以降の内容が行末までコメントとなります。  
コメントの文字コードにはUTF-8が使用できます。  
コメント内容はBASICプログラムの動作には影響しません。

## 動作の中断

シリアルポートから ***「Breakキャラクタ」または「Break信号」*** を受信すると、プログラムおよびコマンドを中断します。  
中断したプログラム動作はResumeコマンドで中断箇所から再開することができます。

ただし、エラー等で処理が中断した場合はResumeコマンドでの再開はできません。  
  
> #### ※「Breakキャラクタ」または「Break信号」  
>BrkCh変数が０のときは「Break信号」のみ、正数のときはその値のASCIIコードの「Breakキャラクタ」と「Break信号」、負数のときは絶対値のASCIIコードの「Breakキャラクタ」で「Break信号」は無効となります。BrkCh変数のデフォルト値は「0x03(Ctrl-C)」です。（詳細は、BrkCh変数の項を参照してください。）  
>LoRa-BASICでは現ボーレートで10ビット分以上のLow状態が「Break信号」となります。（評価ボードに搭載のUSB-UART変換チップによるBreak信号は約1秒間の>Low状態です。）  
>以降、本マニュアルでは ***「Breakキャラクタ/信号」*** と表記します。

## 入れ子

Gosub～Return、For～Next、Do/While～Loopなどの入れ子のスタックはすべて共通で16段階までです。入れ子が16段階を超えると、スタックオーバーフローエラーとなります。

# BASICコマンド

## 分岐コマンド

### Goto

書式：Goto \[ラベル\]

ラベルに指定された行にジャンプする。

ラベルには行番号またはラベル文字列を指定する。

ジャンプ先に行番号を指定する場合は、数値だけでなく変数を使用した式も指定可能。

### Gosub

書式：Gosub \[ラベル\]

ラベルに指定された行のサブルーチンにジャンプします。

ラベルの指定はGotoと同様。

サブルーチンのReturnコマンドで、本コマンドの次のコマンドに戻ります。

### Return

書式：Return

サブルーチンから戻ります。

## 条件分岐、繰り返し

### If～Then～ElseIf～Then～Else～EndIf

書式：If \[式\] Then ～ ElseIf \[式\] Then ～ Else ～ EndIf

式の演算結果が０以外「真」のときはThenに続くコマンド文を実行します。

式の演算結果が０「偽」のときはElseIfの条件判定を、ElseIfが省略された場合はElseに続くコマンド文を実行します。

ElseIf、Else は省略できます。（Ifの後には、ThenとEndIfが必要）

Then、Else 直後に行番号またはラベルを記載すると、Goto
を記述したときと同様の動作となります。

### For～To～Step～Next

書式：For \[変数名=式１\] To \[式２\] Step \[式３\] ～ Next

変数名で指定した変数に式１を代入した後、変数が式２になるまでNextとの間をループします。

プログラムがNextに来る毎に、変数に式３の値が加算されます。

Step 式３は省略可能で、省略された場合は１が指定されたことになります。  
変数名には一般変数と配列変数およびBup1、２のみが指定できます。（その他の変数や式などは指定できません。）

### Do～Loop

書式：Do ～ Loop

Do～Loop を繰り返します。

### Do～Loop While

書式：Do ～ Loop While \[式\]

Do～Loop間を実行した後に式の判定をして、式の結果が0以外「真」のとき、Do～Loop
Whileを繰り返します。

### While～Loop

書式：While \[式\] ～ Loop

式の結果が0以外「真」のとき、While～Loopを繰り返します。

### Exit

書式：Exit

For～Next、Do/While～Loopの繰り返し処理から抜け出し、NextまたはLoop(Loop
While)の直後のコマンドに処理を移します。

入れ子の場合は一番近いループにのみ適用されます。

### Continue

書式：Continue

For～Next、Do/While～Loopのループ内での処理中に、本コマンド以降の処理をスキップして再び繰り返し処理を実行します。

入れ子の場合は一番近いループにのみ適用されます。

## 入出力コマンド

### Print、？

書式：Print \[式\]　または　？\[式\]

数値，文字列を出力します。"Print"のかわりに "?" を代用できます。  
引数には１つまたは複数の式または文字列を指定できます。数値や式は文字列に変換されます。  
複数の式、文字列はコンマやセミコロンで区切って指定します。  
コンマで区切るとタブ出力になり、セミコロンで区切ると続けて出力します。  
最後をセミコロンで終了すると改行しません。  
Printコマンドの式中には、文字列、Chr関数、Form関数などが利用できます。  
Printコマンドの式中で、「'」（クオーテーション）は画面消去のエスケープシーケンスになります。
（例：Print '）  
文字列についての詳細は、「文字列式」の項を参照してください。

### Input

書式：Input \[変数名\]

シリアルポートからの入力を待ち、入力された数値を変数名で指定した変数に格納します。

入力する数値の先頭が＄の場合は、１６進数として扱います。

変数名の後ろに＄をつけると、入力された１文字目のASCIIコードを変数に格納する。  
変数名には一般変数と配列変数およびBup1、２のみが指定できます、（その他の変数や式などは指定できません。）

### Outp

書式：Outp \[式１\] , \[式２\]

GPIOのポート状態を個別または一括で設定します。  
■各ポートを個別に設定する場合  
\[式１\]にポート番号を指定します。（下表のポート以外は指定できません。）

<table style="width:39%;">
<colgroup>
<col style="width: 10%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">ポート番号</th>
<th rowspan="2" style="text-align: center;">ポート名</th>
<th rowspan="2" style="text-align: center;">ピン</th>
</tr>
<tr>
<th style="text-align: center;">10進数</th>
<th style="text-align: center;">16進数</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td style="text-align: center;">$00</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">Pin.32</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">$01</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">Pin.31</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td style="text-align: center;">$04</td>
<td style="text-align: center;">PA04</td>
<td style="text-align: center;">Pin.3</td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td style="text-align: center;">$05</td>
<td style="text-align: center;">PA05</td>
<td style="text-align: center;">Pin.2</td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">$06</td>
<td style="text-align: center;">PA06</td>
<td style="text-align: center;">Pin.7</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">$07</td>
<td style="text-align: center;">PA07</td>
<td style="text-align: center;">Pin.8</td>
</tr>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">$08</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">Pin.9</td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">$09</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">13</td>
<td style="text-align: center;">$0D</td>
<td style="text-align: center;">PA13</td>
<td style="text-align: center;">Pin.15</td>
</tr>
<tr>
<td style="text-align: center;">14</td>
<td style="text-align: center;">$0E</td>
<td style="text-align: center;">PA14</td>
<td style="text-align: center;">Pin.24</td>
</tr>
<tr>
<td style="text-align: center;">15</td>
<td style="text-align: center;">$0F</td>
<td style="text-align: center;">PA15</td>
<td style="text-align: center;">Pin.23</td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">$10</td>
<td style="text-align: center;">PA16</td>
<td style="text-align: center;">Pin.16</td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">$11</td>
<td style="text-align: center;">PA17</td>
<td style="text-align: center;">Pin.17</td>
</tr>
</tbody>
</table>

<table style="width:39%;">
<colgroup>
<col style="width: 10%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">ポート番号</th>
<th rowspan="2" style="text-align: center;">ポート名</th>
<th rowspan="2" style="text-align: center;">ピン</th>
</tr>
<tr>
<th style="text-align: center;">10進数</th>
<th style="text-align: center;">16進数</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">$12</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">Pin.18</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">$13</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">Pin.25</td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">$16</td>
<td style="text-align: center;">PA22</td>
<td style="text-align: center;">Pin.26</td>
</tr>
<tr>
<td style="text-align: center;">23</td>
<td style="text-align: center;">$17</td>
<td style="text-align: center;">PA23</td>
<td style="text-align: center;">Pin.20</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">Pin.28</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">Pin.27</td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">$1B</td>
<td style="text-align: center;">PA27</td>
<td style="text-align: center;">Pin.5</td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">$1C</td>
<td style="text-align: center;">PA28</td>
<td style="text-align: center;">Pin.19</td>
</tr>
<tr>
<td style="text-align: center;">30</td>
<td style="text-align: center;">$1E</td>
<td style="text-align: center;">PA30</td>
<td style="text-align: center;">Pin.29</td>
</tr>
<tr>
<td style="text-align: center;">31</td>
<td style="text-align: center;">$1F</td>
<td style="text-align: center;">PA31</td>
<td style="text-align: center;">Pin.30</td>
</tr>
<tr>
<td style="text-align: center;">34</td>
<td style="text-align: center;">$22</td>
<td style="text-align: center;">PB02</td>
<td style="text-align: center;">Pin.1</td>
</tr>
<tr>
<td style="text-align: center;">35</td>
<td style="text-align: center;">$23</td>
<td style="text-align: center;">PB03</td>
<td style="text-align: center;">Pin.4</td>
</tr>
<tr>
<td style="text-align: center;">54</td>
<td style="text-align: center;">$36</td>
<td style="text-align: center;">PB22</td>
<td style="text-align: center;">Pin.6</td>
</tr>
<tr>
<td style="text-align: center;">55</td>
<td style="text-align: center;">$37</td>
<td style="text-align: center;">PB23</td>
<td style="text-align: center;">Pin.21</td>
</tr>
</tbody>
</table>

\[式２\]には、ポートの状態(Level)を下表のように指定します。

| Level     | ポートの状態          |
|-----------|-----------------------|
| \> 0      | Highを出力            |
| 0         | Lowを出力             |
| -1、\< -3 | ハイインピーダンス    |
| -2        | 入力モード(Pull-up)   |
| -3        | 入力モード(Pull-Down) |

■複数ポートを一括に設定する場合  
\[式1\]の最上位ビット(\$80000000)を 1
にすると、複数ポートをまとめて設定することができます。  
このとき、下表にしたがって設定するポートに対応するビットを1に指定します。

<table style="width:98%;">
<colgroup>
<col style="width: 7%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Bit</th>
<th style="text-align: center;">31</th>
<th style="text-align: center;">30</th>
<th style="text-align: center;">29</th>
<th style="text-align: center;">28</th>
<th style="text-align: center;">27</th>
<th style="text-align: center;">26</th>
<th style="text-align: center;">25</th>
<th style="text-align: center;">24</th>
<th style="text-align: center;">23</th>
<th style="text-align: center;">22</th>
<th style="text-align: center;">21</th>
<th style="text-align: center;">20</th>
<th style="text-align: center;">19</th>
<th style="text-align: center;">18</th>
<th style="text-align: center;">17</th>
<th style="text-align: center;">16</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">ポート</td>
<td rowspan="2">‘1’</td>
<td colspan="4" style="text-align: center;"></td>
<td colspan="4" style="text-align: center;">PB</td>
<td colspan="7" style="text-align: center;">PA</td>
</tr>
<tr>
<td style="text-align: center;">x</td>
<td style="text-align: center;">x</td>
<td style="text-align: center;">x</td>
<td style="text-align: center;">x</td>
<td style="text-align: center;">23</td>
<td style="text-align: center;">22</td>
<td style="text-align: center;">03</td>
<td style="text-align: center;">02</td>
<td style="text-align: center;">31</td>
<td style="text-align: center;">30</td>
<td style="text-align: center;">28</td>
<td style="text-align: center;">27</td>
<td style="text-align: center;">25</td>
<td style="text-align: center;">24</td>
<td style="text-align: center;">23</td>
</tr>
<tr>
<td style="text-align: center;">Bit</td>
<td style="text-align: center;">15</td>
<td style="text-align: center;">14</td>
<td style="text-align: center;">13</td>
<td style="text-align: center;">12</td>
<td style="text-align: center;">11</td>
<td style="text-align: center;">10</td>
<td style="text-align: center;">9</td>
<td style="text-align: center;">8</td>
<td style="text-align: center;">7</td>
<td style="text-align: center;">6</td>
<td style="text-align: center;">5</td>
<td style="text-align: center;">4</td>
<td style="text-align: center;">3</td>
<td style="text-align: center;">2</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td rowspan="2" style="text-align: center;">ポート</td>
<td colspan="16" style="text-align: center;">PA</td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">19</td>
<td style="text-align: center;">18</td>
<td style="text-align: center;">17</td>
<td style="text-align: center;">16</td>
<td style="text-align: center;">15</td>
<td style="text-align: center;">14</td>
<td style="text-align: center;">13</td>
<td style="text-align: center;">09</td>
<td style="text-align: center;">08</td>
<td style="text-align: center;">07</td>
<td style="text-align: center;">06</td>
<td style="text-align: center;">05</td>
<td style="text-align: center;">04</td>
<td style="text-align: center;">01</td>
<td style="text-align: center;">00</td>
</tr>
</tbody>
</table>

※ポート状態の指定は、個別指定の時と同じです。

>注意：  
>「[4.4](#外部ピンの制御機能)
>[外部ピンの制御機能](#外部ピンの制御機能)」の表にあるLED/UART/UART2/I2C/SPI/XTAL32などの機能で共用するポートは、その機能が使用中でもOutpコマンドで上書きされます。これらのポートを使用する際はハードウェア構成等にご注意ください。  
>また、PA04,PA05,PA06,PA14などシステムで使用するポートについてはできるだけ使用しないことを推奨します。  
>特に、PA04,PA05はUARTのポートとなっているので、Outpコマンドで使用するとUARTの制御が無効になります。  
>PA18,PA19はLoRaの送受信時にOutpコマンドにかかわらず変化します。（Ctrl変数で無効にできます。）

例：
```
 >Outp 8,1  
 OK  
 >Outp 9,0  
 OK  
 >Outp 9,-1  
 OK  
 >Outp \$80000060,1  
 OK
```

### Pwm

書式 ： Pwm \[式１\] , \[式２\] , \[式3\]

 ポートにPwmを出力します。

\[式1\]には、出力ポートの番号(port)を指定します。（下表のポート以外は指定できません。）  
この引数は省略できません。

\[式2\]には、Pwmの周期(period)を指定します。省略または０以下を指定するとPwm出力を停止します。  
周期 (Hz) = 2MHz/(period + 1)

\[式３\]には、Pwm出力をHighにする期間(width)を指定します。widthを指定するときは\[式２\](period)の指定も必要です。  
Duty (%) = width / (period + 1)  
widthを省略または負数に指定したときは、「width = (period + 1)/2」(Duty=50%) となります。

<table style="width:92%;">
<colgroup>
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 8%" />
<col style="width: 9%" />
<col style="width: 20%" />
<col style="width: 23%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">ポート番号</th>
<th rowspan="2" style="text-align: center;">ポート名</th>
<th rowspan="2" style="text-align: center;">ピン</th>
<th rowspan="2" style="text-align: center;">グループ</th>
<th rowspan="2">period/ widthの<br />
指定範囲</th>
<th rowspan="2">備考</th>
</tr>
<tr>
<th style="text-align: center;">10進数</th>
<th style="text-align: center;">16進数</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">$08</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">Pin.9</td>
<td rowspan="4" style="text-align: center;">0</td>
<td rowspan="6"><p>0～16777215</p>
<p>(24bit ： $00FFFFFF)</p></td>
<td rowspan="2"></td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">$09</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">$12</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">Pin.18</td>
<td>送信LEDと共用</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">$13</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">Pin.25</td>
<td>受信LEDと共用</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">Pin.28</td>
<td rowspan="2" style="text-align: center;">1</td>
<td rowspan="2"></td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">Pin.27</td>
</tr>
<tr>
<td style="text-align: center;">0</td>
<td style="text-align: center;">$00</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">Pin.32</td>
<td rowspan="2" style="text-align: center;">2</td>
<td rowspan="2"><p>0～65535</p>
<p>(16bit ： $0000FFFF)</p></td>
<td rowspan="2">外部XTAL32k端子と共用</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">$01</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">Pin.31</td>
</tr>
</tbody>
</table>

グループ毎にperiodの値は共通です。同グループのポートに別のperiodを指定したときは、最後に指定した値がグループ内の全ポートに適用されます。  
period, widthはポートによって指定可能範囲が異なります。範囲を超えてもエラーにはなりませんが正しいPwm出力にはなりません。  
Pwm出力を停止したとき、ポートはGPIO出力モードのままになります。

>注意事項 ：  
>- PwmコマンドでPwm出力中のポートにOutp、Inpコマンドを使うと、Pwm出力が停止してそのコマンドが有効になります。
>その後にPwm出力を再開する場合は、Pwmコマンドで一旦Pwmを停止してから再度Pwmの出力を指定してください。
>（Pwmを停止せずにPwmコマンドを使用してもポートには反映されません。）  
>- PA18、PA19は送受信LEDのポートと共用です。  
>このポートにPwmを出力すると送受によるLED点灯時にPwm出力が止まります。  
>（Ctrlビットの設定で送受信LEDを無効にできます。）  
>- PA00、PA01は外部XTAL32kの端子と共用です。  
>このポートの端子にXTAL32kを接続している場合は、このポートをPwmに使用しないでください。

例
```
 >Pwm 8,5000  
 OK  
 >Pwm 8  
 OK  
 >Pwm 18,10000,1000
```

---

## 実行制御コマンド

### End

書式：End

プログラムの実行を終了して、REPLモードの入力待ちに戻ります。

Resumeコマンドでの再開はできません。  
プログラムがエンドレスループなどで実行を終了しない構成の場合は、必ずしもプログラムの最後にEndを入れる必要はありません。メインプログラム＋サブルーチンのような構成で、メインプログラムの最後でそのまま終了せずに直下のサブルーチンに飛び込むことを防止する目的ではメインプログラム最後のEndコマンドは必須です。

### Stop

書式：Stop

プログラムの実行を中断します。  
「Breakキャラクタ/信号」によるプログラムの中断と同様の動作となります。

Resumeコマンドで、このStopコマンドで中断した次のコマンドからの再開が可能です。

### Resume

書式：Resume

Stopコマンドまたは、シリアルポートからの「Breakキャラクタ/信号」で中断した処理を再開します。

エラー発生によって中断した場合はResumeコマンドで再開することはできません。  
REPLモードで中断した場合はResumeコマンドでの再開はできません。

### Pause

書式：Pause

シリアルポートから１文字が入力されるまで待ちます。

シリアルポートからの「Breakキャラクタ/信号」によって中断が可能です。  
（この場合、BASICプログラムの実行中はBASICプログラムも中断します。Resumeコマンドでの再開可能です。）

### Delay

書式 ： Delay time

time ：
指定した時間（1/1000秒単位）だけ動作を停止し、時間が経過すると動作を再開します。  
timeに指定できる値は「0～2,147,483,647
(31bit=\$7FFFFFFF)」です。(負数および範囲外を指定するとエラーになります。)

シリアルポートからの「Breakキャラクタ/信号」によってDelayコマンドは中断します。  
（この場合、BASICプログラムの実行中はBASICプログラムも中断します。Resumeコマンドでの再開可能です。）

### Sleep

書式 ： Sleep \[time\] \[,mode\]

省電力モードで待機します。

timeで指定された値によって以下のように動作します。

<table style="width:95%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 27%" />
<col style="width: 55%" />
</colgroup>
<thead>
<tr>
<th>time</th>
<th>Sleep復帰時間</th>
<th>PA06ピンによる復帰条件</th>
</tr>
</thead>
<tbody>
<tr>
<td>&gt; ０</td>
<td rowspan="2">timeで指定された値の絶対値の時間（秒単位）が経過</td>
<td>mode=0または省略：PA06無効 / mode≠0：mode引数で指定</td>
</tr>
<tr>
<td>&lt; ０</td>
<td rowspan="2">mode 引数で指定する</td>
</tr>
<tr>
<td>= ０<br />
省略時</td>
<td>時間指定なし(無期限)</td>
</tr>
</tbody>
</table>

Sleep状態中はシリアルポートが無効になっているので、「Breakキャラクタ/信号」では復帰しません。  
Sleepコマンドで「PA06ピンによる復帰」が有効に指定されると、Inp関数やOutpコマンドにかかわらず、PA06ポートは強制的に入力ポートに再設定されます。SleepコマンドにPA06を使用する場合は、他の目的でPA06を使用しないようにしてください。また、Sleep状態になるとPA14は強制的に出力ポートに設定され、High状態となります。（Ctrl変数の設定によって、Sleep時のPA14の極性と制御を無効にすることができます。）  
timeに指定できる値は「-2,097,151～2,097,151」です。（範囲外を指定してもエラーにはなりませんが、正しい動作にならないことがあります。)  
  
mode引数にはPA06ピンによるSleepからの復帰条件を指定します。mode引数が省略時されたときは0となります。

<table style="width:93%;">
<colgroup>
<col style="width: 12%" />
<col style="width: 16%" />
<col style="width: 43%" />
<col style="width: 20%" />
</colgroup>
<thead>
<tr>
<th>mode</th>
<th>復帰条件</th>
<th>説明</th>
<th>ピン Pull-Up/Down</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">０</td>
<td rowspan="2">Ctrl変数の$200に従う</td>
<td>Ctrl 変数の$200が0：立ち上がりエッジ</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>Ctrl 変数の$200が1：立ち下がりエッジ</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>１</td>
<td>立ち上がりエッジ</td>
<td>Low→Highの変化</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>２</td>
<td>立ち下りエッジ</td>
<td>High→Lowの変化</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>３</td>
<td>両エッジ</td>
<td>Low→HighおよびHigh→Lowの変化</td>
<td>なし</td>
</tr>
<tr>
<td>４</td>
<td>Highレベル</td>
<td>Highレベル検出</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>５</td>
<td>Lowレベル</td>
<td>Lowレベル検出</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>上記以外</td>
<td colspan="3">エラー</td>
</tr>
</tbody>
</table>

modeが４、５のHigh/Lowレベル検出では、Sleepコマンド実行時点でPA06ピンの状態が既に復帰条件のレベルのときは、Sleepコマンド実行直後にSleepコマンドから復帰します。  
  
Sleep中のポート状態は下表のようになります。

<table style="width:93%;">
<colgroup>
<col style="width: 12%" />
<col style="width: 13%" />
<col style="width: 20%" />
<col style="width: 45%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">ポート名</th>
<th>名称</th>
<th>Sleep中の状態</th>
<th>備考</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">PA06</td>
<td>WAKEUP</td>
<td>入力(弱Pull-Down)</td>
<td>復帰条件に設定されたときのみ<br />
Ctrl変数でUpエッジに設定時は弱Pull-Up</td>
</tr>
<tr>
<td style="text-align: center;">PA14</td>
<td>ACTIVE</td>
<td>High出力</td>
<td>Ctrl変数でLow出力または無効に設定可能</td>
</tr>
<tr>
<td style="text-align: center;">PA04</td>
<td>UART-TX</td>
<td>Hi-Z ※1</td>
<td rowspan="2">Inp,Outpで実行後は状態維持<br />
（Sleep解除時にUART機能に戻ります）</td>
</tr>
<tr>
<td style="text-align: center;">PA05</td>
<td>UART-RX</td>
<td>Hi-Z ※2</td>
</tr>
<tr>
<td style="text-align: center;">PA22</td>
<td>UART2-TX</td>
<td>Hi-Z ※１</td>
<td rowspan="2">UART2としての使用時のみ<br />
未使用時およびInp,Outpで実行後は状態維持</td>
</tr>
<tr>
<td style="text-align: center;">PA23</td>
<td>UART2-RX</td>
<td>Hi-Z ※２</td>
</tr>
<tr>
<td style="text-align: center;">PA16</td>
<td>I2C-SDA</td>
<td>Hi-Z</td>
<td rowspan="2">I2Cとしての使用時のみ<br />
未使用時は状態維持</td>
</tr>
<tr>
<td style="text-align: center;">PA17</td>
<td>I2C-SCL</td>
<td>Hi-Z</td>
</tr>
<tr>
<td style="text-align: center;">上記以外</td>
<td></td>
<td>状態維持</td>
<td></td>
</tr>
</tbody>
</table>

> ※１：Baud/Ubaud変数に送信ポートのPull-Upを指定したときは弱プルアップ  
> ※２：Baud/Ubaud変数に受信ポートのPull-Downを指定したときは弱プルダウン  
   
Sleepコマンドでは以下のクロックが使用されます

| Xtal32k変数 | Ctrl変数   | 使用されるクロック | 精度           | 消費電力 |
|-------------|------------|--------------------|----------------|----------|
| 0           | \$00000000 | 内部低電力32KHz    | -3.0～+3.9%    | 超低電力 |
| 0           | \$00000040 | 内部32KHz          | -0.6～+0.8%    | 低電力   |
| 1           | N/A        | 外部32KHz          | デバイスに依存 | 低電力   |

※外部32KHzは外部ピンにXtal32kを接続した場合のみ使用可能です。  
  
>注意事項：  
>Sleep/Deepコマンド実行時は低消費電力状態で動作しているので外部環境からの電気的ノイズ等の影響を受けやすく、使用環境によってはSleep/Deepから復帰しないなどの不具合が発生する場合があります。  
>外部環境からの影響を低減するため、未使用ポートはHi-Zのままにせず、OutpコマンドまたはInp関数を使用してポート状態を出力やプルアップ/プルダウンに設定するか、ハードウェア的にGNDやVDDに接続することのいずれかを推奨します。

本デバイスが起動した直後の各ポートはHi-Zになっています。（SYSTEMで使用するポートを除きます。）

### Deep

書式：Deep \[time\] \[,mode\]

超省電力モード(Deep Sleep)で待機します。  
timeで指定された値によって以下のように動作します。

<table style="width:95%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 27%" />
<col style="width: 55%" />
</colgroup>
<thead>
<tr>
<th>time</th>
<th>Sleep復帰時間</th>
<th>PA06ピンによる復帰条件</th>
</tr>
</thead>
<tbody>
<tr>
<td>&gt; ０</td>
<td rowspan="2">timeで指定された値の絶対値の時間（秒単位）が経過</td>
<td>mode=0または省略：PA06無効 / mode≠0：mode引数で指定</td>
</tr>
<tr>
<td>&lt; ０</td>
<td rowspan="2">mode 引数で指定する</td>
</tr>
<tr>
<td>= ０<br />
省略時</td>
<td>時間指定なし(無期限)</td>
</tr>
</tbody>
</table>

mode引数にはPA06ピンによるDeepからの復帰条件を指定します。mode引数が省略時されたときは0となります。

<table style="width:93%;">
<colgroup>
<col style="width: 12%" />
<col style="width: 16%" />
<col style="width: 43%" />
<col style="width: 20%" />
</colgroup>
<thead>
<tr>
<th>mode</th>
<th>復帰条件</th>
<th>説明</th>
<th>ピン Pull-Up/Down</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">０、３</td>
<td rowspan="2">Ctrl変数の$200に従う</td>
<td>Ctrl変数の$200が0：Highレベル検出</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>Ctrl変数の$200が1：Lowレベル検出</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>１、４</td>
<td>Highレベル</td>
<td>Highレベル検出</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>２、５</td>
<td>Lowレベル</td>
<td>Lowレベル検出</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>上記以外</td>
<td colspan="3">エラー</td>
</tr>
</tbody>
</table>

Sleepコマンドとの違い  
- Sleepコマンドよりも更に消費電流を落とした、超省電力モード(Deep Sleep)になります。
- Deepコマンドからの復帰はシステムリセットになります。  
- メモリー(RAM)の内容は保存されずに消去されます。  
- PA06ピンは「レベル」検出です。（エッジトリガーには設定できません。）  
- Deepコマンド実行時にPA06が既に復帰条件レベルのときはすぐにDeepから復帰してシステムリセットになります。  
- 内部32Kは使用できません。（外部32Kまたは内部低電力32Kが使用可能です。）  
 
 上記以外の動作はSleepコマンドと同様です。  
   
>#### 消費電力について  
>Sleepは解除からすぐに動作状態に復帰しますが、Deepの解除はリセットからの再起動でBASICプログラムを自動起動するまでに約６秒間の待機時間があり、この分の消費電力がSleepよりも余計に必要となります。  
>したがって、Deepの時間によってはSleepのほうが結果的に省電力になる場合があります。  
>
>リセット後のBASIC自動起動待機中はIDLE相当の消費電流なので以下のような式になります。   
>　｛0.7mA（IDLEの消費電流） x 6 sec｝ / ｛ 1.8uA（Sleepの消費電流） - 0.6uA（Deepの消費電流） ｝  
> 　＝ 4,200 uAsec / 1.2uA ＝ 3,500 sec ＝ 58.3 min ≒ 1 hour  
>
> おおよそ１時間以上のDeepではSleepよりも省電力に効果がありますが、1時間未満の場合はSleepのほうが省電力となります。
> （ただし、周辺回路やポート等の状態を考慮しない場合での概算です。）  
   
 Deepコマンドでは以下のクロックが使用されます。

| Xtal32k変数 | Ctrl変数 | 使用されるクロック | 精度           | 消費電力 |
|-------------|----------|--------------------|----------------|----------|
| 0           | N/A      | 内部低電力32KHz    | -3.0～+3.9%    | 超低電力 |
| 1           | N/A      | 外部32KHz          | デバイスに依存 | 低電力   |

> ※外部32KHzは外部ピンにXtal32kを接続した場合のみ使用可能です。

### Reset

書式：Reset

本デバイスのシステムリセットを実行します。

### Catch

書式：Catch \[ラベル\]

エラー発生時の処理を定義します。  
BASICプログラム実行中にエラーが発生したとき、\[ラベル\]で指定された行に処理を移します。  
\[ラベル\]には行番号またはラベル文字列を指定します。また、行番号には数式の使用も可能です。  
\[ラベル\]に０を指定すると、エラー処理の定義を取り消します。  
\[ラベル\]行に処理が移ると、Catchの設定は無効となります。したがって、エラー処理中のエラーではBASICプログラムが終了します。ただし、エラー処理の完了後に通常処理へ復帰するときなどは、再度Catchコマンドを使用することでエラー処理の再定義が可能です。  
発生したエラーはError変数で、エラー発生行はError#で取得できます。（詳細は「エラーメッセージ」の章を参照してください。）

本コマンドのREPLモードでの実行は無視されます。  
BASICプログラムを中断するための「Breakキャラクタ/信号」も対象となることに注意してください。（エラーコードがBreakのときに中断処理がないと手動でプログラムを停止することができなくなります。）

例：
```
 >>10 Catch 60
 >>20 Do
 >>30 Print "Val=";:Input A
 >>40 @\[256/A\]=A
 >>50 Loop
 >>60 If Error=3 Then Print "Over !":Goto 50 EndIf
 >>70 Error#=Error
 >>run
 Val=3
 Val=2
 Val=1
 Over !
 Val=0
 Division by 0 error in 40  
 OK
```

## データー関連コマンド

### Data

書式：Data　\[式１\], \[式２\]・・・

Readコマンドで読み込む数値を記載します。（数値だけでなく、関数を含む演算式の記述も可能です。）

１つ以上の式をカンマで区切って記述します。カンマが連続またはカンマで終端した場合は暗黙で０が指定されたこととなります。  
Readコマンドの位置とは関係なく、プログラム中のどこでも配置できます。  
Restoreコマンドの使用例を参考にしてください。

### Read

書式：Read \[変数\]

Dataコマンドで記載された式の数値を、変数に読み込みます。  
変数には一般変数と配列変数およびBup1、２のみが指定できます。（特殊変数は指定できません。）

本コマンドを実行するたびに、Data文に設定された数値をプログラムの先頭に近い順から読み込まれます。

読み込むDataの終端になってからReadするとエラーになります。  
Readコマンドでの読み込み対象となったDataコマンドが原因のエラーでプログラムが中断したときのエラーメッセージにはReadコマンドの行番号が表示されます。  
Restoreコマンドの使用例を参考にしてください

### Restore

書式：Restore　\[ラベル\]

Readコマンドで読み込むDataコマンドの順番を変更します。  
引数の\[ラベル\]を省略または０を指定したときはプログラムの先頭から一番近いDataコマンドの位置になります。  
引数に\[ラベル\]
を指定したときは、その\[ラベル\]のラベル名または行番号の後ろにあって一番近いDataコマンドの位置に変更します。

実行例
```
 >>10 Data A+1,3,I+J
 >>20 For J=1 To 2
 >>30 For I=1 To 3:Read A:Print A,:Next
 >>40 Restore
 >>50 Next
 >>Run
 1 3 4 5 3 5 OK
```

### Aload

書式：Aload

AsaveコマンドでFlashメモリーに保存した配列変数「@(0～255)」を読み込みます。  
全ての配列変数が上書きされます。

### Asave

書式：Asave

全ての配列変数「@(0～255)」をFlashメモリーに保存します。Flashメモリーの内容は上書きされます。  
本コマンドで保存した内容はAloadコマンドで読み出します。  
リセットや電源断でもFlashメモリーに保存した内容は維持されます。

## その他のコマンド

### Randomize

書式：Randomize　\[式\]

乱数の種を設定します。

式の値でRnd()関数が生成する乱数の種を設定します。
式が省略された場合は０が指定されたものとします。

この乱数の種は、デバイスIDからの計算値も使用するので、式の値が同じでも本デバイスの個体毎に異なる値となります。　システム起動時はRandomize
0が実行された状態です

### Swap

書式：Swap　\[変数１\]，\[変数２\]

変数１と変数２の内容を入れ替えます。  
変数１、２には一般変数と配列変数およびBup1、２のみが指定できます。（その他の変数や式などは指定できません。）

### Ver

書式：Ver

ソフトウェアのバージョンを表示します。

## プログラム操作コマンド

### Edit

書式：Edit \[式\]

プログラム入力モードの禁止/許可を設定します。

式が０、または省略されたときは「プログラム入力禁止モード」になります。

式が０以外の時は「プログラム入力許可モード」になります。

システム起動時は「プログラム入力禁止モード」です。  
Pass変数でプロテクトモードに設定されているときは「Permission error」となりEditコマンドは実行できません。

#### 「プログラム入力禁止モード」

このモードはREPLモードでのコマンドプロンプトが「\>」になります。

REPLモードでは、数値から始まる行が入力されてもプログラム行の書き込みはエラーとなり、プログラム操作関連のコマンド（New/Renum/Delete/Psave）実行もエラーになります。

これによって、不用意な数値入力によるプログラムの誤消去や誤登録を防止することができます。

#### 「プログラム入力許可モード」

このモードはREPLモードでのコマンドプロンプトが「\>\>」になります。

REPLモードで、数値から始まる行が入力されると、プログラム行の書き込みとなります。

インデントや不要なスペース等は無視され、コメントとラベル、文字列以外は大文字小文字の区別をしません。プログラム行が書き込まれると、次行にコマンドプロンプトが表示されます。「OK」は表示されません。

入力１行毎に中間コードへの変換が行われるので、連続して行を入力する場合は、コマンドプロンプトが表示されてから次行を入力してください。中間コードへの変換時に文法エラー等があった場合は、その行はプログラムエリアに書き込まれずにエラーメッセージを表示します。

入力されたプログラム行は、行番号の順番にプログラムエリアに書き込まれます。

入力された行番号と同じ行番号のプログラム行が既に書き込まれている場合は、新たに入力されたプログラム行に置き換えられます。行番号に続くコマンド等が無く行番号だけの入力のときは、その行番号の行は削除されます。

### New

書式：New

プログラムと変数をクリアします。　「プログラム入力禁止モード」ではエラーとなります。

プログラムエリアのプログラムを全て削除し、各変数も０に初期化されます。  
（LoRa設定値等の特殊変数、およびCtrl変数で\$40が設定されたときの一般と配列変数は初期化されません。）

### Renum

書式：Renum \[式\]

プログラムの行番号を付け替えます。　「プログラム入力禁止モード」ではエラーとなります。

プログラムエリアのプログラム行の行番号を、式で指定された番号からはじまって10ずつ増加するように付け替えます。式は省略可能で、省略時は10と仮定されます。

このコマンドでは行番号のみを付け替えるので、GotoやGosubの飛び先はそのままとなります。

### Delete

書式：Delete \[\[ラベル１\] ，\[ラベル２\]\]

プログラムを削除します。　「プログラム入力禁止モード」ではエラーとなります。

プログラムエリアのプログラムを\[ラベル１\]から\[ラベル２\]の範囲で削除します。  
\[ラベル１\]を単独で指定した場合は\[ラベル１\]のプログラム行のみとなります。（範囲指定ではなくなります。）  
\[ラベル１\]にカンマを付けると、範囲指定の開始行を指定したことになります。  
\[ラベル１\]を省略してカンマのみの場合は\[ラベル１\]にプログラムの先頭を指定したことになります。  
\[ラベル２\]には範囲指定の最終行を指定します。  
\[ラベル２\]を省略した場合は、\[ラベル２\]にプログラムの最後を指定したことになります。

範囲指定で、\[ラベル１\]または\[ラベル２\]が行番号の場合は、その行番号のプログラム行が存在しなくても、その行番号の範囲となります。  
\[ラベル１\]と\[ラベル２\]の両方を省略することはできません。

### Pload

#### FLASHメモリからの読み込み

書式：Pload \[！\[ラベル\]\]

FLASHメモリーに保存されたプログラムをプログラムエリアに読み込みます。  
RAM上にあるプログラムは上書きされます。  
FLASHメモリーにプログラムが保存されていないときは”Program not exist
error” になります。

“Pload！”
を指定すると、プログラムをFLASHから読み込んだ後、そのプログラムを実行します。Runコマンド同様にラベル指定も可能です。

#### 文字列（BASE64）からの復元

書式：Pload＆ \[＋\]

“Pload＆”を指定すると、FLASHメモリーではなく、”Psave
&”で出力したBASE64文字列から復元します。

“Pload &”を実行すると入力待ちになるので、”Psave
&”で出力された複数行の文字列を順次入力します。  
（”Psave&”の出力では先頭にクオーテーション「‘」が付きますが削除しても問題ありません。）  
改行を連続して入力すると、入力を終了してプログラムの復元を開始します。（現プログラムは消去されます。）  
”Psave&”で保存したときと同じPassキーとAesKeyが設定されている必要があります。PassキーとAesKeyが異なるプログラムを入力しても復元できません。  
Passキーに０以外が設定されたプログラムが復元された場合は、「プロテクトモード」および「プログラム入力禁止モード」の状態に関わらずPsaveが可能となります。  
“Pload&＋”とすると追加モードとなって、入力して復元されたプログラムが現プログラムの最後尾に追加されます。ただし、行番号での並べ替えはなく、同一の行番号があってもそのまま追加されます。  
プログラム実行中は、追加モードのみコマンド実行が可能です。

#### LoRaでダウンロード

書式：Pload％ \[＋\]　\[式１\]

“Pload％”を指定すると、”Psave％”を実行する別のデバイスからLoRa経由でプログラムをダウンロードします。  
“Pload％”を実行するとLoRaの受信待ちになります。別のデバイスで”Psave％”を実行してプログラムを送信してください。ダウンロードの状態が表示されます。ダウンロードが完了するとコマンドを終了します。  
\[式１\]にタイムアウト時間を秒単位で指定します。タイムアウト時間内にダウンロードが完了しないとタイムアウトとなります。\[式1\]を省略したときはタイムアウトがありません。（タイムアウト時間が短いとダウンロードを完了できません。）  
AesKeyが有効になっているとAesKeyで暗号化されるので、”Psave％”の送信側と同じAesKeyを設定する必要があります。AesKeyが異なると送受信ができずにエラーになります。  
“Pload％＋”とすると追加モードとなって、ダウンロードしたプログラムが現プログラムの最後尾に追加されます。ただし、行番号での並べ替えはなく、同一の行番号があってもそのまま追加されます。  
プログラム実行中は、追加モードのみコマンド実行が可能です。  
※注意事項  
LoRaWANモードでは実行できません。（Invalid mode error になります。）  
送受信側のChとGid、受信側Ownと送信側Dstが一致している必要があります。  
Sf＝７、Bw＝125KHz、Cr=4/5
に固定です。通常のRecvコマンドでは送受信できません。  
電波状況によってはダウンロードが中断または停止する場合があります。このとき、タイムアウトの時間内であれば、送信側が”Psave％”コマンドを再実行することでダウンロードの再スタートが可能です。  
技適の制限により連続送信ができないため、ブロックの送信間隔が長くなることがあります。（特に上位側チャンネルを使用する場合）

### Psave

#### FLASHメモリへの保存

書式：Psave \[＊\] \[式\]

プログラムエリアのプログラムをFLASHメモリーに保存します。　「プログラム入力禁止モード」ではエラーとなります。

FLASHメモリーに保存することができるプログラムは１つのみです。

FLASHメモリーに保存したプログラムは、電源断やシステムリセットでも消去されません。  
\[式\]を省略または0を指定すると、FLASHメモリーにプログラムを保存します。このとき、RAM上のプログラム領域にプログラムがないときは”Program
not exist error” になります。  
\[式\]に負数を指定したときは、FLASHメモリーのプログラムを消去します。  
\[式\]に１以上を指定すると”Parameter error”になります。

“Psave＊” を指定すると
FLASHへの保存後にプログラムをプロテクトモードに設定します。
”Psave:Pass=-1”
と同等の動作となるため、プロテクト状態を保存するにはSsaveコマンドの実行が必要です。  
※“Pload&”で、Passキーに０以外が設定されたプログラムが復元された場合は、「プロテクトモード」および「プログラム入力禁止モード」の状態に関わらずPsaveが可能となります。（Passキーが負数の場合に限り、Psaveと同時にSsaveが実行されます）

#### 文字列（BASE64）による保存

書式：Psave＆ \[\[ラベル１\]，\[ラベル２\]\]

“Psave&”を指定すると、FLASHメモリーではなく、プログラムをBASE64でエンコードされた複数行の文字列に変換して出力します。（出力文字列の先頭にクオーテーション「‘」が付きますが削除しても問題ありません。）  
\[ラベル１\]と\[ラベル２\]には、出力するプログラムの範囲を行番号またはラベル名で指定することができます。指定方法はListコマンドと同様です。  
表示文字列は乱数を含めて暗号化されるため、同じプログラムでもPsave&を実行の度に異なる文字列になります。  
この文字列は、上記の乱数以外に、PassキーおよびAesKey（AesKeyが有効の場合）も使用して暗号化されますので、“Pload&”で読み込んで復号するときは、PassキーとAesKeyが一致している必要があります。  
“Psave &” コマンドは
　「プログラム入力禁止モード」に関係なく実行可能です。

#### LoRaでアップロード

書式：Psave％ \[\[ラベル１\]，\[ラベル２\]\]

“Psave％”を指定すると、”Pload％”を実行中の別のデバイスにLoRa経由でプログラムをアップロードします。  
\[ラベル１\]と\[ラベル２\]には、アップロードするプログラムの範囲を行番号またはラベル名で指定することができます。指定方法はListコマンドと同様です。  
“Psave％”を実行するとプログラムの送信を開始するので、それに先駆けてアップロード先のデバイスでは”Pload％”コマンドを実行して待ち受けておいてください。  
アップロードが開始されると状態が表示されます。アップロード完了するとコマンドを終了します。  
アップロードでは、プログラムをブロック単位で送信して、受信側から応答を待ってから次のブロックを送信します。応答がない場合は何度か再送しますが、応答エラーの場合はエラー表示をしてコマンドを終了します。  
“Psave％” コマンドは
　「プログラム入力禁止モード」に関係なく実行可能です。

注意事項は”Pload％“を参照してください。  
  
Pass変数でプロテクトモードに設定されているときは「Permission
error」となりPsaveコマンドは実行できません。

### Run

書式：Run \[ラベル\]

プログラムエリアのプログラムを実行します。  

\[ラベル\]を省略した場合は、プログラムを先頭から実行します。

\[ラベル\]を指定した場合は、指定した行番号またはラベルからプログラムを実行します。

実行に先立って各変数は０に初期化されます。（LoRa設定値等の特殊変数、およびCtrl変数で\$40が設定されたときの一般と配列変数は初期化されません。）

プログラムの実行中は「RUNモード」になります。  
プログラムの実行が終了すると、「REPLモード」に戻ります。


### List

書式：List \[\[ラベル１\] ，\[ラベル２\]\]

プログラムエリアのプログラムを\[ラベル１\]から\[ラベル２\]の範囲で行番号順に表示します。  
\[ラベル１\]を単独で指定した場合は\[ラベル１\]のプログラム行のみとなります。（範囲指定ではなくなります。）  
\[ラベル１\]にカンマを付けると、範囲指定の開始行を指定したことになります。  
\[ラベル１\]を省略してカンマのみの場合は\[ラベル１\]にプログラムの先頭を指定したことになります。  
\[ラベル２\]には範囲指定の最終行を指定します。  
\[ラベル２\]を省略した場合は、\[ラベル２\]にプログラムの最後を指定したことになります。

範囲指定で、\[ラベル１\]または\[ラベル２\]が行番号の場合は、その行番号のプログラム行が存在しなくても、その行番号の範囲となります。  
\[ラベル１\]と\[ラベル２\]の両方を省略した場合は、全プログラム行となります。

例：
```
 List        : 全プログラム  
 List 10     : 行番号10のみ  
 List 10,    : 行番号10からプログラムの最後まで  
 List ,20    : プログラムの先頭から行番号20まで  
 List 10,20  : 行番号10から行番号20までを表示
```
Pass変数でプロテクトモードに設定されているときは「Permission error」となりListコマンドは実行できません。

---

# BASIC関数

## 一般関数

一般関数は、主に数値演算のために使用します。

### Rnd

書式 ： Rnd ( 式 )  
式が０のときは、０～2,147,483,647 (\$7FFF FFFF) の乱数が戻ります。  
式が０以外のときは、式の絶対値を超えない０以上の乱数が戻ります。  
例 ： 
```
 A=Rnd(B) ← 0≦A≦\|B\| の乱数
```

### Abs

書式 ： Abs ( 式 )

式の絶対値が戻ります。  
例 ：
```
 A=Abs(B) ← A=\|B\|
```

### Atan2

書式 ： Atan2(式１、式２)

式２／式１の逆正接が戻ります。  
計算結果は－πから＋πラジアンですが、本関数からの戻り値は計算結果の1000000倍の整数値となります。  
したがって、本関数の計算結果を 17453 (1000000π/180)
で割ると「度(degree)」に換算することができます。  
引数１が０の場合の戻り値は０となります。（"Division by 0 error"
にはなりません。）  
一般的な逆正接関数は計算結果が－π/2から＋π/2ラジアンですが、本関数は式１が負数の場合でも正しい角度を計算することができます。  
例 ： A=Atan2(B,C) ← A=tan<sup>-1</sup>(C/B)

### Sqrt

書式：Sqrt ( 式 )

式１の平方根が戻ります。　式１が負数のときは2,147,483,647 (\$7FFF FFFF)
が戻ります。  
例 ：
```
 A=Sqrt(B) ← A=√B
```

### Pow

書式 ： Pow ( 式１、式２ )

式１の値を、式２の乗数で累乗した値が戻ります。　式２が０以下のときは1が戻ります。  
演算子の ”＊＊” を使用しても、Pow関数と同様の累乗演算ができます。  
例 ：
```
 A=Pow(B,C) または A=B\*\*C ← A=B<sup>C</sup>
```

### Int8

書式 ： Int8 ( 式 )

式の下位８ビットだけを変換対象とし、それより上位のビットは０とみなします。下位８ビットを符号付８ビット値として解釈した値を返します。  
式１の下位８ビットが0～127のときはそのままの値を、128～255のときは-128～-1を返します。

### Int16

書式 ： Int16 ( 式 )

式の下位１６ビットだけを変換対象とし、それより上位のビットは０とみなします。下位１６ビットを符号付１６ビット値として解釈した値を返します。  
式１の下位１６ビットが0～32,767のときはそのままの値を、32,768～65,535のときは-32,768～-1を返します。

### Int24

書式 ： Int24 ( 式 )

式の下位２４ビットだけを変換対象とし、それより上位のビットは０とみなします。下位２４ビットを符号付２４ビット値として解釈した値を返します。  
式１の下位２４ビットが0～8,388,607のときはそのままの値を、8,388,608～16,777,215のときは-8,388,608～-1を返します。

### Bswap16

書式 ： Bswap16 ( 式 )

３２ビットの値を、１６ビット毎に上位８ビットと下位８ビットを入れ替えます。  
エンディアンの変換に利用できます。  
例 ：
```
 A=Bswap16(\$12345678) ← A=\$34127856 となります
```

### Bswap32

書式 ： Bswap32 ( 式 )

３２ビットの値を、８ビット単位で上位と下位を入れ替えます。  
エンディアンの変換に利用できます。  
例 ：
```
 A=Bswap32(\$12345678) ← A=\$78563412 となります
```

---

## 入力関数

入力関数は、デバイスのGPIOポートからデジタル値またはアナログ値を取得するために使用します。

### Inp

書式 ： Inp ( 式 )

GPIOのポートの状態を取得します。  
式にはポート番号を指定します。（下表にあるポート番号のみ指定可能です。）  
戻り値は、ポートの入力状態がLowのときは０に、Highのときは１になります。

<table style="width:39%;">
<colgroup>
<col style="width: 8%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 11%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">ポート番号</th>
<th rowspan="2" style="text-align: center;">ポート名</th>
<th rowspan="2" style="text-align: center;">ピン</th>
</tr>
<tr>
<th style="text-align: center;">10進数</th>
<th style="text-align: center;">16進数</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td style="text-align: center;">$00</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">Pin.32</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">$01</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">Pin.31</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td style="text-align: center;">$04</td>
<td style="text-align: center;">PA04</td>
<td style="text-align: center;">Pin.3</td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td style="text-align: center;">$05</td>
<td style="text-align: center;">PA05</td>
<td style="text-align: center;">Pin.2</td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">$06</td>
<td style="text-align: center;">PA06</td>
<td style="text-align: center;">Pin.7</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">$07</td>
<td style="text-align: center;">PA07</td>
<td style="text-align: center;">Pin.8</td>
</tr>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">$08</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">Pin.9</td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">$09</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">13</td>
<td style="text-align: center;">$0D</td>
<td style="text-align: center;">PA13</td>
<td style="text-align: center;">Pin.15</td>
</tr>
<tr>
<td style="text-align: center;">14</td>
<td style="text-align: center;">$0E</td>
<td style="text-align: center;">PA14</td>
<td style="text-align: center;">Pin.24</td>
</tr>
<tr>
<td style="text-align: center;">15</td>
<td style="text-align: center;">$0F</td>
<td style="text-align: center;">PA15</td>
<td style="text-align: center;">Pin.23</td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">$10</td>
<td style="text-align: center;">PA16</td>
<td style="text-align: center;">Pin.16</td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">$11</td>
<td style="text-align: center;">PA17</td>
<td style="text-align: center;">Pin.17</td>
</tr>
</tbody>
</table>

<table style="width:39%;">
<colgroup>
<col style="width: 8%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 11%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">ポート番号</th>
<th rowspan="2" style="text-align: center;">ポート名</th>
<th rowspan="2" style="text-align: center;">ピン</th>
</tr>
<tr>
<th style="text-align: center;">10進数</th>
<th style="text-align: center;">16進数</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">$12</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">Pin.18</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">$13</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">Pin.25</td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">$16</td>
<td style="text-align: center;">PA22</td>
<td style="text-align: center;">Pin.26</td>
</tr>
<tr>
<td style="text-align: center;">23</td>
<td style="text-align: center;">$17</td>
<td style="text-align: center;">PA23</td>
<td style="text-align: center;">Pin.20</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">Pin.28</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">Pin.27</td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">$1B</td>
<td style="text-align: center;">PA27</td>
<td style="text-align: center;">Pin.5</td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">$1C</td>
<td style="text-align: center;">PA28</td>
<td style="text-align: center;">Pin.19</td>
</tr>
<tr>
<td style="text-align: center;">30</td>
<td style="text-align: center;">$1E</td>
<td style="text-align: center;">PA30</td>
<td style="text-align: center;">Pin.29</td>
</tr>
<tr>
<td style="text-align: center;">31</td>
<td style="text-align: center;">$1F</td>
<td style="text-align: center;">PA31</td>
<td style="text-align: center;">Pin.30</td>
</tr>
<tr>
<td style="text-align: center;">34</td>
<td style="text-align: center;">$22</td>
<td style="text-align: center;">PB02</td>
<td style="text-align: center;">Pin.1</td>
</tr>
<tr>
<td style="text-align: center;">35</td>
<td style="text-align: center;">$23</td>
<td style="text-align: center;">PB03</td>
<td style="text-align: center;">Pin.4</td>
</tr>
<tr>
<td style="text-align: center;">54</td>
<td style="text-align: center;">$36</td>
<td style="text-align: center;">PB22</td>
<td style="text-align: center;">Pin.6</td>
</tr>
<tr>
<td style="text-align: center;">55</td>
<td style="text-align: center;">$37</td>
<td style="text-align: center;">PB23</td>
<td style="text-align: center;">Pin.21</td>
</tr>
</tbody>
</table>

ポート番号に以下の値を加算すると、ポートをプルアップ/プルダウンに設定できます。

<table style="width:67%;">
<colgroup>
<col style="width: 44%" />
<col style="width: 11%" />
<col style="width: 11%" />
</colgroup>
<thead>
<tr>
<th rowspan="2">ポートの状態</th>
<th colspan="2" style="text-align: center;">加算する値</th>
</tr>
<tr>
<th style="text-align: center;">16進数</th>
<th style="text-align: center;">10進数</th>
</tr>
</thead>
<tbody>
<tr>
<td>ハイインピーダンス (プルアップ/プルダウンしない)</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td>プルダウン</td>
<td style="text-align: center;">$100</td>
<td style="text-align: center;">256</td>
</tr>
<tr>
<td>プルアップ</td>
<td style="text-align: center;">$200</td>
<td style="text-align: center;">512</td>
</tr>
</tbody>
</table>

>注意事項：  
>「[4.4外部ピンの制御機能](#外部ピンの制御機能)」の表にあるLED/UART/UART2/I2C/SPI/XTAL32などの機能で共用するポートは、その機能が使用中でもInp関数で上書きされます。これらのポートを使用する際はハードウェア構成等にご注意ください。  
>また、PA04,PA05,PA06,PA14などシステムで使用するポートについてはできるだけ使用しないことを推奨します。  
>特にPA04,PA05はUARTのポートとなっているので、Inp関数で使用するとUARTの制御が無効になります。  
>PA18,PA19はLoRaの送受信時に出力になります。（Ctrl変数で無効にできます。）

### Adc

書式 ： Adc ( 式 )

AdcポートからAD変換された値を取得します。

式には下表に記載のAdc番号を指定してください。（それ以外の値を指定するとエラーになります）

<table style="width:91%;">
<colgroup>
<col style="width: 7%" />
<col style="width: 6%" />
<col style="width: 8%" />
<col style="width: 13%" />
<col style="width: 16%" />
<col style="width: 19%" />
<col style="width: 18%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">Adc番号</th>
<th rowspan="2">名前</th>
<th colspan="2">入力ソース</th>
<th rowspan="2">戻り値</th>
<th rowspan="2">リファレンス</th>
</tr>
<tr>
<th style="text-align: center;">10進</th>
<th style="text-align: center;">16進</th>
<th>GPIO</th>
<th>ピン</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">$06</td>
<td>AN6</td>
<td>PA06</td>
<td>Pin.7</td>
<td rowspan="8"><p>0～4095</p>
<p>Adc変換の値がそのまま戻ります。</p></td>
<td rowspan="8">VDD</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">$07</td>
<td>AN7</td>
<td>PA07</td>
<td>Pin.8</td>
</tr>
<tr>
<td style="text-align: center;">10</td>
<td style="text-align: center;">$0A</td>
<td>AN10</td>
<td>PB02</td>
<td>Pin.1</td>
</tr>
<tr>
<td style="text-align: center;">11</td>
<td style="text-align: center;">$0B</td>
<td>AN11</td>
<td>PB03</td>
<td>Pin.4</td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">$10</td>
<td>AN16</td>
<td>PA08</td>
<td>Pin.9</td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">$11</td>
<td>AN17</td>
<td>PA09</td>
<td>Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td>AN24</td>
<td colspan="2">CPU内部の温度センサの計測値</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td>AN25</td>
<td colspan="2">内部リファレンス電圧（1.00V）</td>
</tr>
<tr>
<td style="text-align: center;">26</td>
<td style="text-align: center;">$1A</td>
<td>AN26</td>
<td colspan="2">CPUコア電圧（Typ:0.9～1.2V）</td>
<td rowspan="2">1mV単位の計測値</td>
<td rowspan="2">内部リファレンス<br />
(1.00V)</td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">$1B</td>
<td>AN27</td>
<td colspan="2">VDD電圧</td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">$1C</td>
<td>AN28</td>
<td colspan="2">CPU内部の温度センサの変換値</td>
<td>温度(0.1℃単位)</td>
<td></td>
</tr>
</tbody>
</table>

>注意事項 ：  
>Inp関数のポート番号とAdc番号は、同じGPIOピンでも指定する値は異なります。  
  
#### GPIOピン (AN6～17）
入力ソースがGPIOピンの場合、Adc関数を実行するとポートはアナログ入力に設定されます。  
ポートの電圧は以下の式で求めることができます。  
ポート電圧\[mV\] = Adc(n) \* Adc(27) / 4095 ※n=6,7,10,11,16,17

#### CPU内部の温度センサ (AN24,28)
AN24はCPU内部温度が同じであってもデバイスの個体毎に異なる値となります。  
また、センサの計測値はチップ内部の温度のために周辺の温度とは同じになりません。  
AN28はAN24とAN27から取得した値を基にしてMPU固有の補正値（チップメーカーがMPU毎に書き込み）で計算した値です。そのため、AN28の値も周辺温度とは異なる値になります。  
必要に応じて、使用環境で実際に測定した温度を基にして誤差等を補正してください。  
CPU内部の温度センサには以下のような測定誤差があります。(MPUのデーターシートより)

| 条件     | 最小値 | 最大値 | 単位 |
|----------|--------|--------|------|
| T \< 0℃  | -12    | 14     | ℃    |
| T \> 0℃  | -8     | 9      | ℃    |
| T \> 30℃ | -5     | 5      | ℃    |

### Inkey

書式 ： Inkey、Inkey ( 式 )

シリアルポート(UART)の入力バッファから取得した文字のASCIIコードです。  
式にはタイムアウト時間をmsec単位で指定します。  
(式)を省略、または0を指定した場合はタイムアウトなしですぐに戻ります。

式に負数を指定したときはタイムアウトなしで入力があるまで待ちます。  
入力バッファが空でタイムアウトしたときは-1が戻ります。  
BASIC実行中等でシリアルポートに先行入力があったときは、バッファ内の入力を順次取得します。

入力バッファから値を取得すると、入力バッファからその値がなくなります。  
エコーバックはありません。

---

## 文字列関数

文字列関数は文字列を扱うために使用します。  
LoRa-BASICは整数値のみしか扱うことができず、文字列の使用は限定的です。  
特定のコマンドや変数を使用するときに文字列の使用が可能です。  
Print、Lprint、Sendコマンドの引数やTDX、Rxd変数などでは、数値以外に文字列での指定が可能です。

また、いくつかの変数は文字列操作が基本のものがあります。  

### Chr

書式 ： Chr ( 式 )

式の下位8ビットをそのまま文字列中に出力します。  
Printコマンドなどで特定のキャラクタコードを利用したい場合などに使用します。

### WChr

書式 ： WChr ( 式 )

式の下位１６ビット(Little endian)をそのまま文字列中に出力します。

Printコマンドや文字列作成に利用できます。

送信データなどにWORD（２byte）サイズのバイナリデーターを利用したい場合などに使用します。

### LChr

書式 ： LChr ( 式 )

式の下位３２ビット(Little endian)をそのまま文字列中に出力します。  
Printコマンドや文字列作成に利用できます。

送信データなどにLONG
WORD（４byte）サイズのバイナリデーターを利用したい場合などに使用します。

### MChr

書式 ： MChr ( 式 )

式の下位２４ビット(Little endian)をそのまま文字列中に出力します。  
Printコマンドや文字列作成に利用できます。

送信データなどにMIDDLE
WORD（３byte）サイズのバイナリデーターを利用したい場合などに使用します。

### Form

書式：Form(書式文字列、式)

書式文字列の指定に従って、式の数値を文字列に出力します。

書式文字列で使用する書式指定文字は以下のとおり。

　D ：10進数（デフォルト）  
　X ：16進数（0～9、A～F）  
　x ：16進数（0～9、a～f ）  
　0 ：リーディングゼロ（無指定の場合は空白文字）  
　1～9：出力桁数指定（無指定の場合は可変長）  
　- ：符号用に１文字分を左端に追加します。  
　+ ：正数の場合に＋符号を付加します。  
　無指定の場合、デフォルトの10進数・可変長となります。  
  
Formの簡易版として、１０進数文字列への変換には「０(式１，式２)」、１６進数文字列への変換は「＄(式１，式２)」のような記述ができます。（詳細は「[6.6.1](#数値文字列)[数値→文字列](#数値文字列)」を参照してください。）

例 ：
```  
 Print Form("",-1234)      --> "-1234" を表示  
 Print Form("X04", 1234)   --> "04D2" を表示  
 Lprint Form("-05", 1234)  --> "01234" をLCD表示  
 Send Form("-5",-1234)     --> "- 1234" をLoRaで送信  
 Txd=Form("-+5",1234)      --> "+ 1234" をLoRa送信バッファに設定
```

### Gets

書式 ： Gets、Gets ( 式 )

シリアルポート(UART)の入力バッファから１行分の文字列を入力します。  
CrまたはLFが入力されるまでを行として、１行分が入力されるとその文字列を出力します。  
文字列にはCr/LFなどの制御文字コード(\$00～\$1F)は含みません。  
式にはタイムアウト時間をmsec単位で指定します。  
(式)を省略、または0を指定した場合は１行分の入力があるまでタイムアウトなしで待ちます。  
タイムアウト時間内に１行分の入力が終了しないと、入力途中でも空文字列を出力して本関数を終了します。  
文字列の最大長は255バイトで、これを超えた入力は破棄されます。  
UARTから入力されたキャラクタは、Echo変数の設定に応じて、エコーバックされます。  
   
#### ***電文フォーマット***
 Gets！を指定すると、電文フォーマットによる入力となり、STX(02) ～ ETX(03)
 で囲まれた電文データの入力を待ちます。（戻り文字列にはSTX/ETXを含みません。）  
 Gets！で電文フォーマットが指定されたときに限り、Breakキャラクタによるプログラムの中断ができません。（Break信号は有効です。)

### Datetime

書式 ： Datetime ( 式 )  
GPS制御のDatetimeコマンドを文字列中で使用すると、シリアル秒の文字列変換関数として機能します。  
シリアル秒から変換された文字列は ”YY/MM/DD hh:mm/ss” の形式になります。

シリアル秒はTimeVal関数、またはClock変数で取得できます。

 例 ：
 ```
 >^=Datetime(116858096):? “20“^  
 2023/09/14 12:34:56
```

>※文字列中での使用ではなく、コマンドとしての使用方法は「GPSコマンド」の「Datetime」を参照してください。

### Bcc

 書式 ： Bcc ( 文字列 )  
 文字列の水平パリティを取得します。  
 文字列のキャラクタを8bit単位にすべてXORした値が戻ります。

例 ：
```
>? $(Bcc("ABCDEFG"))  
40
```

### Sum

書式 ： Sum ( 文字列 )  
文字列の総和を取得します。  
文字列のキャラクタを8bit単位にすべて加算した値が戻ります。

例 ：
 ```
 > ? $(Sum("ABCDEFG"))  
 1DC
```

### ToInt

書式 ： ToInt( 文字列 \[、\[オフセット\] \[、\[文字数\]\]\] )  
文字列中の指定部分を数値に変換します。  
引数の\[オフセット\]で、変換を開始する文字列中の位置を指定できます。（省略時または０は文字列の先頭となります。）  
引数の\[文字数\]で、変換する数値の文字数を指定できます。（省略時または０は文字列の最後までとなります。）  
引数の\[オフセット\]と\[文字数\]に負数を指定することエラーになります。  
対象の数値文字列が＄から始まるときは１６進数となり、それ以外では１０進数となります。\[オフセット\]が文字列長を超えたとき、および数値以外の文字列は０となります。変換途中に数値以外の文字がある場合はその直前までの数値が変換対象です。１０進数文字列では先頭の”－”と”＋”の文字は有効です。

例 ：
```
 >? ToInt("ABC-1234",3,4)  
 -123
```

## 日付時刻関数

日付や時刻を扱うために使用します。

### TimeVal

書式 ： TimeVal( 式 )  
配列変数の日付(年、月、日)と時刻（時、分、秒）をシリアル秒に変換します。  
式には、日付と時刻を格納した配列変数の先頭インデックス値（0～249）を指定します。  
変換されたシリアル秒は、「2020/1/1, 00:00:00」からの経過秒数です。  
各値の配列変数への格納順はDatetimeコマンドに準じます。  
変換元の日付と時刻がDatetimeコマンドの「変換後の値」以外の場合、正しい変換結果にはなりません。

# 文字列変数

汎用的な文字列変数には「 ^ （キャレット）」と「 .
（ドット）」が使用できます。  
それぞれの文字列変数は使用方法が異なります。  
文字列変数に一般の数値変数を代入すると、数値を文字列変換して文字列変数に設定します。  
逆に、数値変数に文字列変数を代入すると、文字列を数値変換して設定します。このときに「式」を記述する際は、式の文字列以降はすべて文字列式の扱いになり、前部は数値式となります。数値以外の文字列は０になります。  
ただし、文字列式を括弧で囲むとその部分は数値として扱われるので、文字列式を数式の途中に記述することができます。

例 ：
```
 A=100+”1234”;”567”;^    <-- OK  
 A=”1234”+100            <-- Error  
 A=(“1234”;”567”)+100+^  <-- OK
```

## ^（キャレット）文字列変数

一時的に文字列を格納するのに用います。  
Printコマンドなどの引数と同様に、文字列式を記述して代入と参照が可能です。  
数値演算のXOR演算子とは異なるものとして区別されますが、数値演算子と明示的に区別する必要があるときは、”;”セミコロンで区切ってください。
（例：Print A;^;B）  
なお、^ (キャレット)文字列のみの特殊機能として以下のものがあります。

#### ^ 文字列のデコード

「 ^ (キャレット)」
の前にデコード記号（＄、％、＆）を付けると文字列をデコードします。  
例 ： Print \$^

#### ^ 文字列内の文字コード

^ (キャレット)に続いて配列変数の様に括弧を付けると、^
(キャレット)文字列の指定位置の文字コードが参照できます。  
　0が先頭文字となります。（現在の文字列長を超えた位置を指定したときは-1を返します。）  

例 ：
```
 A=^[1]  
```
文字列内の指定位置に文字コードを設定することができます。現在の文字列長以上を指定するとエラーになります。  

例 ：
```
 ^[1]=A
```

#### ^ の文字列長

^ (キャレット)文字列の文字コードの参照で位置指定を省略すると、^
(キャレット)文字列の文字列長の参照となります。  

例 ：
```
> L=^[]
```

文字列長を指定することができます。現在の文字列長よりも大きな値を指定すると延長部分はスペースになります。

例 ：
```
 ^[]=10  
```

文字列長に負数を指定すると、その文字数を文字列の先頭から削除します。

例 ：
```
 ^[]=-3
```

使用例：
```
 >B=10203  
 OK  
 >^=B“XYZ“  
 OK  
 >Print ^[1],^[8]  
 48 -1  
 OK  
 >Print ^  
 10203XYZ  
 OK  
 >^=Form(“X“,^[1]+32)  
 OK  
 >^=^^  
 OK  
 >Print ^  
 5050  
 OK  
 >A=B+^“40“  
 OK  
 >Print A  
 515243  
 OK  
 >^=A  
 OK  
 >^=“\[“$Gets“]“  
 123  
 OK  
 >Txd=^  
 OK  
 >Send  
 OK  
 >Print Txd  
 [QRC123]  
 OK
```

## .（ドット）文字列変数

直前に使用した文字列を表します。文字列のコピーではなくその文字列そのものを参照するので、文字列式中にこの文字列変数の参照があると、その時点までに生成された文字列は有効のまま、文字列長は直前に使用した文字列の文字列長になり、文字列の参照位置は先頭に戻ります。  
Printを改行ありで使用したとき、その文字列を、「 .
（ドット）」で参照すると改行コード(CR/LF)も含まれます。

```
 >Print “XYZ”  
 XYZ  
 OK  
 >Print “A”.;
 AYZ  
 OK  
 >^=.”123”Chr(40)  
 OK  
 >Print .;  
 AYZ  
 123(OK  
 >#=.  
 OK  
 >Print Txd(7)  
 9  
 OK  
 >Send  
 OK
```

## その他文字列変数

送受信バッファのTxd、Rxd、#
を添え字無しで参照や設定（Txdのみ）をすると、文字列変数としての扱いとなります。  
また、Auto変数も文字列変数となります。

#  システム変数と定数

本BASICには、システムで予約された変数と定数があります。  
文字列以外のシステム変数と定数は、通常の変数や数値と同様に計算式やコマンド引数などに使うことができます。

## システム定数

システム定数は、システム動作中に値が変化しない定数となります。ただし、デバイス毎に異なる値となる定数もあります。
このシステム定数は参照のみが可能で、値の代入や設定はできません。  
■システム定数の一覧と説明

<table style="width:93%;">
<colgroup>
<col style="width: 15%" />
<col style="width: 18%" />
<col style="width: 59%" />
</colgroup>
<thead>
<tr>
<th>定数名</th>
<th>内容</th>
<th>説明</th>
</tr>
</thead>
<tbody>
<tr>
<td>Null</td>
<td>ヌル</td>
<td>数値のときは０と同等、文字列のときは文字列長０の文字列</td>
</tr>
<tr>
<td>False</td>
<td>偽</td>
<td>０と同等</td>
</tr>
<tr>
<td>True</td>
<td>真</td>
<td>１と同等</td>
</tr>
<tr>
<td>Sn</td>
<td>シリアル番号</td>
<td>本デバイスのシリアル番号です。デバイス毎に固有の値です。<br />
本デバイスに貼られているシリアル番号の数値です。</td>
</tr>
<tr>
<td>DevId</td>
<td>デバイス固有のID文字列</td>
<td>Snから生成されるデバイス毎に固有の値で、IEEEのEUI-64フォーマットです。<br />
上位36ビットのOUI-36は「70-B3-D5-59-E」で、IEEEがデバイスのハードウェアメーカーに正式に割り当てた固有値です。<br />
Printコマンド等、文字列の使用箇所で16文字の16進数文字列として参照できます。変数へ代入など、数値として使用されるときは、DevIdの下位20ビットの値となります。（Snとは異なる値です）</td>
</tr>
</tbody>
</table>

## システム変数

システム変数は一般の変数と異なり、それぞれ特定の機能を有しているため、目的に応じて使用します。  
システム変数には、それぞれに設定や参照、保存と復元が可能なものや不可能なものがあります。  
■システム変数一覧

<table style="width:96%;">
<colgroup>
<col style="width: 18%" />
<col style="width: 33%" />
<col style="width: 15%" />
<col style="width: 16%" />
<col style="width: 12%" />
</colgroup>
<thead>
<tr>
<th>変数名</th>
<th>内容</th>
<th style="text-align: center;">設定と参照<br />
(✕は参照のみ)</th>
<th style="text-align: center;">保存と復元<br />
(Ssave/Sload)</th>
<th style="text-align: center;">デフォルト値</th>
</tr>
</thead>
<tbody>
<tr>
<td>Echo</td>
<td>エコーバック</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">1</td>
</tr>
<tr>
<td>Auto</td>
<td>自動実行文字列</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">空文字列</td>
</tr>
<tr>
<td>Baud</td>
<td>ボーレート</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">115,200</td>
</tr>
<tr>
<td>BrkCh</td>
<td>Breakキャラクタ/信号</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">3 (Ctrl-C)</td>
</tr>
<tr>
<td>Xtal32k</td>
<td>低速クロック設定</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Hop</td>
<td>転送回数</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td>FwdConf</td>
<td>転送パラメーター</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">-1</td>
</tr>
<tr>
<td>Tick</td>
<td>システムカウント</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Clock</td>
<td>RTCクロック</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Milisec</td>
<td>RTCクロックのミリ秒</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Cause</td>
<td>起動原因</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>＿（アンダーバー）</td>
<td>現在行番号</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Inkey</td>
<td>入力文字</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Gets</td>
<td>文字列入力</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>BUP１、２</td>
<td>バックアップ変数</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Ctrl</td>
<td>コントロール変数</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td>Error</td>
<td>エラーコード</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Pass</td>
<td>設定レベル</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">△</td>
<td style="text-align: center;">０</td>
</tr>
</tbody>
</table>

システム変数それぞれの詳細説明は以降を参照してください。

### Echo

シリアルポートに入力された文字のエコーバックを設定します。

- = ０ ： エコーバックなし  
- ≠ ０ ： エコーバックあり  

「エコーバックあり」に設定されていても、認識されない制御コードなどの文字はエコーバックされません。

### Auto

システム起動時に自動実行するBASICコマンドを文字列で設定します。  
通常のコマンド実行と同様にマルチステートメントでの記述が可能です。文字列の最大文字数は６３文字です。  
Printコマンド等で、Auto変数文字列の利用が可能です。

Autoが設定されている場合、通常は起動後に5秒間待機してから実行します。待機中はBREAK操作でAutoの実行を中止できます。  
*Ctrl*の`$04000000`を設定すると、Deepからの復帰による再起動時に限り、この待機を省略してAutoを直ちに実行します。これは、低消費電力でSleepするDeep運用において、起床後の5秒間の待機による無駄な消費電力を抑えるための機能です。電源投入時やDeep復帰以外のリセットでは、従来どおり5秒間待機します。

### Baud

シリアルポートの転送速度（ボーレート）を設定します。

設定可能な値は次の通りです。  
> 300, 600, 1200, 1800, 2400, 3600, 4800, 7200, 9600, 14400, 19200,
> 28800, 38400, 57600, 76800, 115200  

 負数を指定すると、シリアルポートを停止します。  
 「例 ： ”Baud=-1”」  
 （ポート状態はSleep中に準じます。）

設定値に以下の値を加算することでパリティビット、キャラクタビット、ストップビット、受信ポートの状態を指定することができます。

<table style="width:85%;">
<colgroup>
<col style="width: 18%" />
<col style="width: 12%" />
<col style="width: 13%" />
<col style="width: 13%" />
<col style="width: 26%" />
</colgroup>
<thead>
<tr>
<th rowspan="2">項目</th>
<th colspan="2">加算値</th>
<th rowspan="2">内容</th>
<th rowspan="2">備考</th>
</tr>
<tr>
<th>10進数</th>
<th>16進数</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="3">パリティ</td>
<td>０、３</td>
<td>＄００、＄０３</td>
<td>パリティ無し,</td>
<td rowspan="3"></td>
</tr>
<tr>
<td>１</td>
<td>＄０１</td>
<td>奇数パリティ</td>
</tr>
<tr>
<td>２</td>
<td>＄０２</td>
<td>偶数パリティ</td>
</tr>
<tr>
<td rowspan="2">ストップビット</td>
<td>０</td>
<td>＄００</td>
<td>１ビット</td>
<td rowspan="2"></td>
</tr>
<tr>
<td>４</td>
<td>＄０４</td>
<td>２ビット</td>
</tr>
<tr>
<td rowspan="2">キャラクタビット</td>
<td>０</td>
<td>＄００</td>
<td>８ビット</td>
<td rowspan="2"></td>
</tr>
<tr>
<td>８</td>
<td>＄０８</td>
<td>７ビット</td>
</tr>
<tr>
<td rowspan="2">受信ポート</td>
<td>０</td>
<td>＄００</td>
<td>Hi-Z</td>
<td rowspan="2">Ctrl変数での設定も有効<br />
Sleep/Deep中を含む</td>
</tr>
<tr>
<td>１６</td>
<td>＄１０</td>
<td>Pull-Down</td>
</tr>
<tr>
<td rowspan="2">送信ポート</td>
<td>０</td>
<td>＄００</td>
<td>Hi-Z</td>
<td rowspan="2">Sleep/Deep中の状態</td>
</tr>
<tr>
<td>３２</td>
<td>＄２０</td>
<td>Pull-Up</td>
</tr>
</tbody>
</table>

例 ：
```
 ”Baud=326” （300bps, 7bit, parity-even）」
```

Baud変数を設定すると即時にシリアルポートに反映され、新ボーレートで”OK”が出力されます。  
”Baud=9600：Ssave”
のようにマルチステートメントによるコマンド記述をお勧めします。  

>注意事項 ：  
>DefaultコマンドでBaudをデフォルト値に再設定したときはボーレートがシリアルポートに反映されません。この場合はSsaveコマンドで保存した後の再起動時に設定が反映されます。  
>受信ポートの設定は、０を指定してもCtrl変数による受信ポートのPull-Down設定が有効になります。  
>送信ポートの設定は、Sleep/Deep中のポート状態を指定します。  
>Outpコマンドで送受信ポートを設定すると、Sleep/Deepが終了するまではその状態が維持されます。

### BrkCh

プログラムの動作を中断するために使用する「Breakキャラクタ」のASCIIコードとBreak信号を設定します。

設定可能な値は-255～255です。

<table style="width:50%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 23%" />
<col style="width: 12%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">BrkCh変数</th>
<th>Breakキャラクタ</th>
<th style="text-align: left;">Break信号</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">＝０</td>
<td>無効</td>
<td rowspan="2" style="text-align: left;">有効</td>
</tr>
<tr>
<td style="text-align: center;">＞０</td>
<td rowspan="2" style="text-align: left;">絶対値のASCIIコード</td>
</tr>
<tr>
<td style="text-align: center;">＜０</td>
<td style="text-align: left;">無効</td>
</tr>
</tbody>
</table>

注意：

PA05(UART-RX)は本デバイスの内部でプルアップしていませんので、PA05ピンを外部回路でプルアップしていないときにPA05と外部デバイスとの接続がなくなる等によって入力状態が不安定になると、意図しないBreak信号が認識されてBASICプログラムが中断する可能性があります。この場合、BrkCh変数に負数を指定してBreak信号を無効にするか、受信ポートをPull-Upする（ハードウェアまたはBaud変数の設定）などの対策をしてください。

### Xtal32k

低速クロック(32.768kHz)を以下の種類に設定します。  
＝０ ： 内部Crを使用します（出荷時）  
＝１ ： 外部接続の水晶振動子を使用します

上記以外の値は無視されます。  
Defaultコマンドでは初期化されません。（Defaultコマンドの引数が-1の場合を除く。）  
外部端子に水晶振動子(32.768kHz)を接続したときのみ、”1”（外部）に設定してください。水晶振動子を接続せずに”1”を設定したときはエラーになります。（ハードウェアの状態によってはハングアップする場合があります。）  
詳細は ***「LoRa-BASIC for LRA1 リファレンスマニュアル LoRaWAN編」*** を参照してください。  
水晶振動子の接続方法については「LRA1ハードウェアマニュアル」を参照してください。

### Hop

転送機能有効時の転送回数を設定/参照します。

０～７が設定可能です。  
詳細は「[14.8](#p2p転送機能)
[P2P転送機能](#p2p転送機能)」の章を参照してください。

### FwdConf

転送機能有効時の送信パラメーターを設定/参照します。  
詳細は「[14.8](#p2p転送機能)
[P2P転送機能](#p2p転送機能)」の章を参照してください。

### Tick

システム起動から1/1000秒毎にカウントアップされる値です。  
リセット、電源ON、Deepからの復帰時に０にクリアされます。  
（Sleep中でもTickのカウントは継続します。）  
値の参照と設定が可能です。

### Clock

システム起動から1秒毎にカウントアップされる値です。  
電源ONで０にクリアされます。リセット、Deepからの復帰時はクリアされずにカウントが継続されます。  
（Deep中でもClockのカウントは継続します。）  
Clock変数に値を設定すると、Milisecは０に設定されます。（Milisecは任意の値に設定できません）  
LoRaWANのWan_TimeReq
による時刻取得では自動的にClock変数が設定されますが、この場合に限りMilisecには取得時刻のミリ秒が設定されます。  
（詳細は ***「LoRa-BASIC for LRA1 リファレンスマニュアル LoRaWAN編」*** を参照してください）

### Milisec

直前のClock変数を参照したときのミリ秒（0～999）を取得します。  
Milisec変数を参照する前にClock変数を参照する必要があります。  
値の参照のみで設定はできません。（LoRaWANのWan_TimeReqを除く）  
Clock変数と同様に、リセットやDeepからの復帰時もカウントが継続されます。

>※注意事項  
>Clock変数を連続で取得してもMilisecの値は連続した値にはなりません。  
>４msec程度の期間は同じ値が取得されます。（その間もClockはカウントしています。）

### Cause

システムの起動原因を表す以下の32ビット値です。  
値の参照のみで設定はできません。

下位16ビットはリセットによるシステムの起動原因を表します。

| 下位16ビット | リセット要因                                        |
|--------------|-----------------------------------------------------|
| \$----0001   | Power On Reset                                      |
| \$----0002   | コア電圧低下リセット (1.2V以下)                     |
| \$----0004   | 電源電圧低下リセット (1.6V以下)                     |
| \$----0010   | 外部リセット信号（Reset端子）                       |
| \$----0020   | ウォッチドッグリセット                              |
| \$----0040   | Resetコマンド （BASICエラー後無操作リセットを含む） |
| \$----0180   | Deep WAKEUP端子信号                                 |
| \$----0280   | Deep タイムアウト                                   |

上位16ビットはSleepからの復帰原因を表します。

| 上位16ビット | Sleep起動要因        |
|--------------|----------------------|
| \$0001----   | Sleep WAKEUP端子信号 |
| \$0002----   | Sleep タイムアウト   |

### ＿（アンダーバー）

「＿（アンダーバー）」単体の場合：  
現在の行番号を取得します。（REPLモードのときは０になります。）  
Gotoコマンドなどの分岐先にも使用可能です。  
値の参照のみで設定はできません。  
  
「ラベル」の場合：  
「＿（アンダーバー）」に文字列が続くときは通常のラベルとしてそのラベルの行番号が取得されます。  
プログラム中にこのラベルが無い場合は０になります。  
値の参照のみで設定はできません。

### Inkey

シリアルポート(UART)の入力バッファからキャラクタを取得します。  
Inkey関数でタイムアウトに０を指定したのと同等です。  
Inkey ⇔ Inkey(0)  
詳細は、入力関数のInkeyを参照してください。

### Gets

シリアルポート(UART)の入力バッファから文字列を入力します。  
文字列関数のGetsでタイムアウトに０を指定したのと同等です。  
Gets ⇔ Gets(0)  
詳細は、文字列関数のGetsを参照してください。

### Bup1、Bup2

リセット時にバックアップされるBup1とBup2の独立した２つの変数です。  
BUP１とBup2変数はリセットによって初期化されずにリセット発生直前の値を保ちます。  
ただし、電源が断たれると値が不定となるため、電源ONリセットの時は０にクリアされます。

本変数は符号付３２ビットの値で一般変数と同様に扱うことができます。  
複合代入による設定と単項演算子によるインクリメント、デクリメントが可能です。  
For、Read、Input、Swap、Bmeコマンドにおける変数に指定が可能です。

### Ctrl

各種コントロールを設定します。

範囲 ： なし（32bit）  
本システムの動作や送受信コマンドの振る舞いを指定することができます。  
各機能と指定するビットは下表のようになります。（LoRaWAN関連は灰色文字）

<table style="width:95%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 80%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;"><strong>　値(16進数)</strong></th>
<th><strong>機　能</strong></th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0000 0001</td>
<td>Recvコマンドで、Rssi値の表示をしない</td>
</tr>
<tr>
<td style="text-align: center;">0000 0002</td>
<td>Recvコマンドで、送信元IDの表示をしない</td>
</tr>
<tr>
<td style="text-align: center;">0000 0004</td>
<td>Recvコマンドで、行頭に @ の表示をしない</td>
</tr>
<tr>
<td style="text-align: center;">0000 0008</td>
<td>Recvコマンドで、CRCエラーを表示する</td>
</tr>
<tr>
<td style="text-align: center;">0000 0010</td>
<td>起動時にバージョン等を表示しない</td>
</tr>
<tr>
<td style="text-align: center;">0000 0020</td>
<td>LED(PA18, PA19)の制御をしない</td>
</tr>
<tr>
<td style="text-align: center;">0000 0040</td>
<td>Sleepで内部32Kクロックを使用する（内部低電力32Kクロックを使用しない）</td>
</tr>
<tr>
<td style="text-align: center;">0000 0080</td>
<td>BASIC実行エラー後に180秒間無操作でリセットする</td>
</tr>
<tr>
<td style="text-align: center;">0000 0100</td>
<td>Commコマンドは送信のみ（受信しない）</td>
</tr>
<tr>
<td style="text-align: center;">0000 0200</td>
<td>WAKEUP(PA06)の極性を反転する（デフォルトではHighでWakeupする）</td>
</tr>
<tr>
<td style="text-align: center;">0000 0400</td>
<td>ACTIVE(PA14)の極性を反転する（デフォルトではLowがActive状態）</td>
</tr>
<tr>
<td style="text-align: center;">0000 0800</td>
<td>Rxd(PA05)をPull-Downにする。（デフォルトはHi-Z）</td>
</tr>
<tr>
<td style="text-align: center;">0000 1000</td>
<td>ACTIVE(PA14)を制御しない (Outp, Inpコマンドは使用可能)</td>
</tr>
<tr>
<td style="text-align: center;">00002000</td>
<td>LED(PA18)とLED(PA19)を入れ替える</td>
</tr>
<tr>
<td style="text-align: center;">0000 4000</td>
<td>LED(PA18)を送信BUSYでHighにする （デフォルトは送信中にHigh）</td>
</tr>
<tr>
<td style="text-align: center;">0000 8000</td>
<td>外部XTAL32kをタイマーに使用しない（Deep/SleepのタイムアウトとClockのみに使用）</td>
</tr>
<tr>
<td style="text-align: center;">0001 0000</td>
<td>PB23にRFの1MHzを出力する。（RF動作時のみ）</td>
</tr>
<tr>
<td style="text-align: center;">0002 0000</td>
<td>PA22に内部タイマーの1kHzを出力する</td>
</tr>
<tr>
<td style="text-align: center;">0004 0000</td>
<td>転送機能を有効にする（P2Pモードのみ）</td>
</tr>
<tr>
<td style="text-align: center;">0008 0000</td>
<td>転送機能を拡張（転送機能が無効：転送パケットのみ受信／転送機能が有効：重複受信容認）</td>
</tr>
<tr>
<td style="text-align: center;">0010 0000</td>
<td>MACコマンド送受信時にJoin状態をFLASH保存する。（FLASH保存が有効のとき）</td>
</tr>
<tr>
<td style="text-align: center;">0020 0000</td>
<td rowspan="2">LoRaWANでのJoin状態の保存方法（2ビットの論理和）<br />
0=すべて保存／2=FLASHにはJoin情報のみ／4=RAMへの保存のみ／6=保存しない</td>
</tr>
<tr>
<td style="text-align: center;">0040 0000</td>
</tr>
<tr>
<td style="text-align: center;">0080 0000</td>
<td>LoRaWANで送信フレームのUpCounterを固定する（Unjoin状態でのReset後から有効）</td>
</tr>
<tr>
<td style="text-align: center;">0100 0000</td>
<td>LoRaWANのUp/Down Dwell
を「制限なし」にする（Unjoin状態でのReset後から有効）</td>
</tr>
<tr>
<td style="text-align: center;">0200 0000</td>
<td>LoRaWANのSyncwordをPrivateにする（Reset後から有効）</td>
</tr>
<tr>
<td style="text-align: center;">0400 0000</td>
<td>Deepからの復帰による再起動時に、消費電力を抑えるため起動待ち時間を省略し、Autoを直ちに実行する</td>
</tr>
<tr>
<td style="text-align: center;">0800 0000</td>
<td rowspan="5">未使用（0に設定してください）</td>
</tr>
<tr>
<td style="text-align: center;">1000 0000</td>
</tr>
<tr>
<td style="text-align: center;">2000 0000</td>
</tr>
<tr>
<td style="text-align: center;">4000 0000</td>
</tr>
<tr>
<td style="text-align: center;">8000 0000</td>
</tr>
</tbody>
</table>

複数の機能を設定するときは、各ビットの論理和を指定してください。  
表中の「値」は16進数で表記していますが、Ctrl変数は一般の変数と同様に10進数での指定も可能です。  
LoRaWAN関連のビットについては ***「LoRa-BASIC for LRA1 リファレンスマニュアル LoRaWAN編」*** を参照してください。

Ctrl変数は特殊変数ですが、複合代入演算子を使用した代入が可能です。  
  
注意 ：
未使用のビットは常に０を設定してください。０以外を設定した場合の動作は保証できません。

### Error

エラーコードの参照またはエラーをスローします。

参照 ： 直前に発生したエラーコードの値です。  
設定 ： 指定エラーコードのエラーをスローします。  
詳細は「[11.3](#error変数) [Error変数](#error変数)」を参照してください。

### Pass

設定レベルを設定します。  
設定値および参照値は以下のようになります。

<table style="width:97%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 11%" />
<col style="width: 16%" />
<col style="width: 58%" />
</colgroup>
<thead>
<tr>
<th>設定値</th>
<th>参照値</th>
<th>レベル</th>
<th>内容</th>
</tr>
</thead>
<tbody>
<tr>
<td>-1</td>
<td>-1</td>
<td>プロテクトモード</td>
<td><p>List、Edit、Psave コマンドがエラーになります。</p>
<p>一旦、Pass変数を-1に設定すると変更ができません。<br />
プロテクトモードはSsaveで保存できます。（保存しないと再起動時にプロテクトモードは解除されます）<br />
Ssaveしたプロテクトモードを解除するにはDefaultコマンドを使用します。（詳細はDefaultコマンドを参照してください）</p></td>
</tr>
<tr>
<td>999999</td>
<td>1</td>
<td>管理レベル</td>
<td>通常レベルで設定/参照ができないLoRaWAN関連変数の設定/参照が可能となります。</td>
</tr>
<tr>
<td>上記以外</td>
<td>0</td>
<td>通常レベル</td>
<td>いくつかのLoRaWAN関連変数を設定/参照できません。</td>
</tr>
</tbody>
</table>

管理レベルで設定/参照できるLoRaWAN関連の変数については、***「LoRa-BASIC for LRA1 リファレンスマニュアル LoRaWAN編」*** を参照してください。  
Pass変数に指定したレベルはSsaveで保存されません。再起動で「通常レベル」になります。

```
 >Pass=999999  
 OK  
 >? Pass  
 1  
 OK  
 >Pass=-1  
 OK  
 >List  
 Permission error  
 OK
```

### Pass. (Passの後にドット)

Psave&/Pload&
でBASE64にエンコード/デコードするときのPassキー（暗号キー）を設定します。  
Passドット変数は設定のみで、参照はできません。（Pass変数とは異なる変数です。）  
Psave&
コマンドで出力されるプログラムのBASE64文字列は次のような設定になります。

<table style="width:60%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 23%" />
<col style="width: 22%" />
</colgroup>
<thead>
<tr>
<th>Passドット</th>
<th>暗号キー</th>
<th>復元時のプロテクト</th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>AesKey</td>
<td rowspan="2">現状維持</td>
</tr>
<tr>
<td>正数</td>
<td rowspan="2">AesKey＆Passキー</td>
</tr>
<tr>
<td>負数</td>
<td>強制する</td>
</tr>
</tbody>
</table>

※AesKeyはAesKeyが有効に設定されている場合のみ使用できます。

「強制プロテクト」が設定されたBASE64文字列をPload&で読み込むと、Pass=-1を実行した場合と同様に、プロテクトモードに設定されます。（負数のPassキーと正数のPassキーは別のキーとして扱います。）

```
 >Pass.=-1234  
 OK  
 >Psave &  
 '2eGfB3lHjOpFFpW3VDdNy+9UaPtJV74SVD1gE+oi6Gt++0ogboY5RQVetLdk0rZ4
 'BKXxp09z1HJnvbWoEVJx6y0jlM8lyFN/1gLh  
 OK  
 >Pload &  
 
 [ここにBASE64を貼り付け]  
 
 OK  
 >List   
 Permission error  
 OK
```

# エラー


## エラーメッセージ

エラーが発生したときは、以下のエラーメッセージを表示して処理を中止します。

プログラムモードのときは、エラーが発生した行の行番号も表示します。

<table style="width:96%;">
<colgroup>
<col style="width: 26%" />
<col style="width: 6%" />
<col style="width: 63%" />
</colgroup>
<thead>
<tr>
<th>エラーメッセージ</th>
<th style="text-align: center;">コード</th>
<th>内容</th>
</tr>
</thead>
<tbody>
<tr>
<td>OK</td>
<td style="text-align: center;">0</td>
<td>エラーなし</td>
</tr>
<tr>
<td>Syntax error</td>
<td style="text-align: center;">1</td>
<td>文法エラーです。認識できないコマンド文や記述ミスがあります。</td>
</tr>
<tr>
<td>Division by 0 error</td>
<td style="text-align: center;">2</td>
<td>０で除算しました。</td>
</tr>
<tr>
<td>Array index over error</td>
<td style="text-align: center;">3</td>
<td>配列の添え字の範囲が負数または255を超えています。<br />
バッファの指定範囲を超えたときもこのエラーとなります。</td>
</tr>
<tr>
<td>Parameter error</td>
<td style="text-align: center;">4</td>
<td>パラメーターが必要なコマンドにパラメーターが指定されていません。<br />
または、パラメーターの範囲を超えた値が指定されました。</td>
</tr>
<tr>
<td>Stack overflow error</td>
<td style="text-align: center;">5</td>
<td>スタックがオーバーしました<br />
Gosub,For,Do,While の入れ子が16段を超えました。</td>
</tr>
<tr>
<td>Can't resume error</td>
<td style="text-align: center;">6</td>
<td>プログラムを Resume コマンドで再開できません。<br />
処理がエラーで中止されたときは再開ができません。</td>
</tr>
<tr>
<td>Label not found error</td>
<td style="text-align: center;">7</td>
<td>Goto, Gosubの指定先のラベル番号が見つかりませんでした。</td>
</tr>
<tr>
<td>Unless from run-mode error</td>
<td style="text-align: center;">8</td>
<td>プログラムモードでは実行できないコマンドです。<br />
Pload,New,Renum,Resumeコマンドはプログラムから実行できません。</td>
</tr>
<tr>
<td>Program area overflow error</td>
<td style="text-align: center;">9</td>
<td>BASICプログラムのメモリー容量がオーバーしました。</td>
</tr>
<tr>
<td>Loop nothing error</td>
<td style="text-align: center;">10</td>
<td>Do/Whileに対応するLoopがありません。</td>
</tr>
<tr>
<td>Endif not found error</td>
<td style="text-align: center;">11</td>
<td>Ifに対応するEndIfがありません。</td>
</tr>
<tr>
<td>Device access error</td>
<td style="text-align: center;">12</td>
<td>I2Cデバイス(LCD,BME280など）のアクセスエラーです。</td>
</tr>
<tr>
<td>Edit mode error</td>
<td style="text-align: center;">13</td>
<td>プログラム編集モードではありません。</td>
</tr>
<tr>
<td>Invalid mode error</td>
<td style="text-align: center;">14</td>
<td>現在のモード(P2PまたはWAN)では操作できません。</td>
</tr>
<tr>
<td>Permission error</td>
<td style="text-align: center;">15</td>
<td>設定レベルが管理レベルではありません。</td>
</tr>
<tr>
<td>Overflow error</td>
<td style="text-align: center;">16</td>
<td>文字列バッファがオーバーしました。</td>
</tr>
<tr>
<td>Unexpected Next error</td>
<td style="text-align: center;">17</td>
<td>Nextに対応するForがありません。</td>
</tr>
<tr>
<td>Unexpected Return error</td>
<td style="text-align: center;">18</td>
<td>Returnに対応するGosubがありません。</td>
</tr>
<tr>
<td>Unexpected Loop error</td>
<td style="text-align: center;">19</td>
<td>Loopに対応するDo/Whileがありません。</td>
</tr>
<tr>
<td>Unexpected Exit error</td>
<td style="text-align: center;">20</td>
<td>Exitに対応するForまたはDo/Whileがありません。</td>
</tr>
<tr>
<td>Unexpected Continue error</td>
<td style="text-align: center;">21</td>
<td>Continueに対応するForまたはDo/Whileがありません。</td>
</tr>
<tr>
<td>Unexpected Read error</td>
<td style="text-align: center;">22</td>
<td>Readコマンドに対するDataがありません。またはDataが終端です。</td>
</tr>
<tr>
<td>Flash write error</td>
<td style="text-align: center;">23</td>
<td>Flashメモリーへの書き込みでエラーが発生しました。</td>
</tr>
<tr>
<td>Program not exist error</td>
<td style="text-align: center;">24</td>
<td>プログラムがありません。</td>
</tr>
<tr>
<td>Break</td>
<td style="text-align: center;">255</td>
<td>「Breakキャラクタ/信号」によって中断されました。</td>
</tr>
</tbody>
</table>

## エラー処理

通常、BASICのコマンド実行でエラーが発生したときはエラーメッセージを表示して処理を中止しますが、Catchコマンドでエラー処理を定義することにより、エラー発生時にユーザー定義のエラー処理ハンドラに処理を移行し、エラー処理の後にBASICプログラムを継続することができます。  
詳細は「Catchコマンド」を参照してください。

## Error変数

Error変数を使用してエラーコードの取得や、エラーをスローすることができます。  
特に、ユーザー定義のエラー処理ハンドラではError変数を使用してのエラー内容に応じた処理に必要です。  
Error変数はエラー処理ハンドラ以外のいつでも参照や設定が可能です。

### Error変数の参照

Error変数を参照すると、最後の発生したエラーコードを取得できます。  
Error#
（#を付加する）として参照すると、最後にエラーが発生した行番号が取得できます。  
（REPLモードでエラーが発生したときのError#は０です。）

### Error変数の設定

Error変数にエラーコードを設定すると、そのエラーコードのエラーをスローします。  
ただし、０を設定するとエラーはスローされずにError変数参照時のエラーコードが０にクリアされます。  
Error#
（#を付加する）にエラーコードを設定すると、最後にエラーが発生した行番号でエラーをスローしたことになります。（任意の行番号を指定することはできません。）  
  
エラーメッセージが定義されていないエラーコードの設定も可能ですが、数値のエラーメッセ―となります。  
  
※エラー処理ハンドラでは、エラー処理対象外のエラーについては
「Error#=Error」 を実行してプログラムを中止することを推奨します。

#  LCD制御

　  
以下のコマンドと変数を使用して評価ボード上のLCDに文字を表示することができます。

I2CでLCDが接続されている必要があります。

## LCDコマンド

### Lclr

書式 ： Lclr

LCDの表示をクリアします。

### Lprint

書式 ： Lprint \[引数\]

LCDに文字列を表示します。  
引数はPrintコマンドに準拠しますが、改行とタブは無視されます。  
表示が１行の桁数を超えても改行しません。

## LCD変数

### Lpos

範囲 ： ０～7、６４～７１  
評価ボード上のLCDに文字列を表示するときの表示位置を指定します。

表示位置と設定値は以下のようになります。

| 1行目 | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   |
|:-----:|-----|-----|-----|-----|-----|-----|-----|-----|
| 2行目 | 64  | 65  | 66  | 67  | 68  | 69  | 70  | 71  |

設定値が設定可能範囲外でもエラーにはなりません。  
Lpos変数の参照時は現在の表示位置となります。（１文字分を表示すると＋１されます。）

### Lcont

範囲 ： １～６３  
評価ボード上のLCDのコントラストを設定します。  
１ ： 薄い ←→ 63 : 濃い

デフォルト値は２５です。  
１以下が指定されると1に、６３以上が指定されると６３に設定されます。  
参照では現在の設定値となります。  
コントラストは電源電圧に依存します。おおよそ「(4.5-Vdd)x20」を設定してください。  
例) Lcont=(4500-Adc(27))/50

#  BME280制御

以下のコマンドを使用して評価ボード上のBME280から気温・湿度・気圧を取得することができます。

I2CでBME280が接続されている必要があります。

## BME280コマンド

### Bme

書式：Bme \[引数１\[，引数２\[，変数名３\]\]\]

評価ボード上のBME280で「気温、湿度、気圧」を計測します。  
引数が省略された場合は、「気温、湿度、気圧」の順に表示します。  
引数を指定したときは、計測値は表示されずに引数に指定した変数名の変数に計測値が設定されます。  
引数１の変数には気温、引数２の変数には湿度、引数３の変数には気圧が入ります。  
引数で指定する変数名には一般変数と配列変数およびBup1、２のみが指定できます。（その他の変数や式などは指定できません。）  
表示または変数に格納される各計測値は以下のようになります。

気温 ： 0.1(℃)単位  
湿度 ： 0.1(%)単位  
気圧 ： 0.1(hPa)単位  

例：
```
 >Bme  
 226 391 10223  
 OK  
 >Bme A,B,C  
 OK  
 >Print A,B,C  
 226 391 10223  
 OK
```

---

# 文字列のエンコーディング、デコーディング

文字列は「16進数」、「BASE64」または「パーセント」でエンコード/デコードすることができます。

## Printコマンドでのエンコーディング

Printコマンドの前に「＆」をつけると、BASE６４でエンコードした文字列を表示します。  
Printコマンドの前に「％」をつけると、パーセントでエンコードした文字列を表示します。  
Printコマンドの前に「＄」をつけると、１６進数でエンコードした文字列を表示します。

```
>&Print "1234"   <---- Base64 decode  
MTIzNA0KOK  
>%Print "1234"   <---- Percent decode  
1234%0d%0aOK  
>\$Print "1234"  <---- Hexdecimal decode  
313233340d0aOK
```

## エンコーディングされた文字列

文字列を以下のように指定すると、エンコードされた文字列として扱います。

```
&"xxxxxxxxxxxx"  <---- Base64 decode
```

```
>Txd=&”MTIzNDU=”  
OK  
>Print Txd  
12345  
OK  
>&Print Txd  
MTIzNDUNCg==OK　　　　<--- 改行コードもエンコードされて、コマンド終了の”OK”が続いて表示  
>Send  
OK  
>Txd=\$”61313233”  
OK  
>Print Txd  
a123  
OK
```

---

# UART2 制御

本デバイスはメインのシリアルポートのほかに、以下の様にPA22とPA23を追加のシリアルポートとして利用することができます。  
LoRa-BASICでは、このPA22と23を使用したシリアル通信機能を「UART2」と表記します。

| ポート名 | 端子   | 名前     | 内容         |
|----------|--------|----------|--------------|
| PA22     | Pin.26 | UART2-TX | デバイスから送信 |
| PA23     | Pin.20 | UART2-RX | デバイスへの受信 |

UART2は、システムリセットや電源ONまたはSleep/Deepからの復帰後に初めてUART2関連のコマンドの実行や変数での入力を実行したときに有効になります。それまではUART2の各ポート(PA22,
PA23)は有効になりません。  
UART2のボーレートはUbaud変数で指定した値となります。（デフォルト値は9600）  
  
本デバイスには、このUART2を制御するコマンドや変数と、UART2にGPSデバイスを接続したときに利用できるGPS用のコマンドがあり、この章ではこのUART2とGPSに関連したコマンドおよび変数について解説します。

## UART2コマンド

### Utrans

書式：Utrans \[式\]  
UART2を透過モードで使用します。  
UART2のボーレートはUbaud変数で指定した値となります。（デフォルト値は9600）  
メインのUARTとUART２を透過的につなげます。UARTから入力された文字はUART2に送信し、UART2から受信した文字はUARTに送信します。  
メインUARTから「Breakキャラクタ/信号」が入力されると本コマンドを終了します。  
また、\[式\]にタイムアウトをmsec単位で指定すると、そのタイムアウト時間内にUARTおよびUART2のどちらからも入力がないとコマンドを終了します。\[式\]が省略されるか0が指定されるとタイムアウトしません。また負数を指定するとエラーになります。UARTまたはUART2のどちらから入力がある度にタイムアウトのカウントが\[式\]の値に戻ります。  
メインUARTとUART2のボーレートは異なっても構いませんが、バッファのオーバーフローに注意してください。  
本コマンド実行中のみUART2のPA22、PA23が有効になり、コマンドが終了すると各ポートはHi-Zになります。

### Uprint

書式：Uprint \[式\]  
UART2に\[式\]の結果を出力します。  
出力先がUART2になる以外は、Printコマンドと同様です。  
Printコマンドと同様に、Uprintコマンドの前にデコード指定子をつけることも可能です。

## UART2変数

UART2に関連する変数には以下のものがあります。

■UART2変数一覧

<table style="width:96%;">
<colgroup>
<col style="width: 18%" />
<col style="width: 33%" />
<col style="width: 15%" />
<col style="width: 16%" />
<col style="width: 12%" />
</colgroup>
<thead>
<tr>
<th>変数名</th>
<th>内容</th>
<th style="text-align: center;">設定と参照<br />
(✕は参照のみ)</th>
<th style="text-align: center;">保存と復元<br />
(Ssave/Sload)</th>
<th style="text-align: center;">デフォルト値</th>
</tr>
</thead>
<tbody>
<tr>
<td>Ubaud</td>
<td>UART2ボーレート</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">9600</td>
</tr>
<tr>
<td>Uinkey</td>
<td>UART2文字入力</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
<tr>
<td>Ugets</td>
<td>UART2文字列入力</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">なし</td>
</tr>
</tbody>
</table>

### Ubaud

UART2のボーレートを設定、参照します。  
設定したボーレートは、Uprint, Uinkey,Ugets, Ugps, Utrans
を実行したときに有効となります。  
設定可能な値はBaud変数と同様です。（詳細はBaud変数を参照してください。）

### Uinkey

書式 ： Uinkey または Uinkey（式）

UART2から１文字分を入力します。  
UART2からの入力された文字のASCIIコードを返し、入力がない場合は-1が返ります。  
UART2からの入力以外はInkey変数と同じです。  
括弧を付けるとInkeyと同様にタイムアウトの指定が可能です。

### Ugets

書式 ： Ugets または Ugets（式）

UART2から文字列を入力します。  
UART2から入力する以外は、Gets変数と同じです。  
括弧を付けるとGets関数と同様に、Ugets関数としてタイムアウトの指定が可能です。  
エコーバックはありません。  
Ugets！を指定すると、電文フォーマットによる入力となります。(UARTからのBreakキャラクタおよびBreak信号によるプログラムの中断は有効です。)

---
# GPS

GPSデバイスをUART2に接続すると、GPSを制御するためのUgpsコマンドの利用が可能です。また、GPSから取得した時刻情報を操作するためのDatetimeコマンドがあります。  
GPSデバイスには一般的なNMEAフォーマットをシリアル出力するものが使用可能です。（ただし、該当するすべてのGPSデバイスの対応を保証するものではありません。）

## Ugps

書式：Ugps \[式１\], \[式２\]  
UART2に接続されたGPSデバイスから時刻、緯度、経度情報を取得します。

\[式１\]にはタイムアウト値(msec単位)を指定します。（省略時および０指定時は、無制限）  
\[式２\]にはHDOP閾値x10を指定します。（省略時は0です）

UART2のボーレートはUbaud変数で指定した値となります。（デフォルト値は9600）  
GPSから取得した値は、以下のように配列変数の０～４に設定されます。

| 設定先 | 　設定される値                                         |
|--------|--------------------------------------------------------|
| ＠(０) | シリアル秒 (UTCの「2020/1/1, 00:00:00」からの経過秒数) |
| ＠(１) | 緯度 (度を1000000倍した値)、南緯は負数                 |
| ＠(２) | 経度 (度を1000000倍した値)、西経は負数                 |
| ＠(３) | 標高 (メートル10倍した値)                              |
| ＠(４) | 水平精度低下率（HDOPを10倍した値)                      |

GPSから有効な測位情報を取得すると、その測位情報を配列変数の@(0～4)に格納します。このとき、その測位情報のHDOPが\[式２\]のHDOP閾値以下のとき
（ただし\[式２\]が０または省略されたときはHDOPに関わらずすぐに）、コマンドを終了します。それまでは、GPSからの測位情報の取得動作を継続します。  
タイムアウト、またはメインUARTから「Breakキャラクタ/信号」が入力されるとコマンドを終了します。  
GPSから有効な測位情報が取得されていれば、タイムアウトや「Breakキャラクタ/信号」による終了時は、最新の有効な位置情報が@(0～4)に格納されますが、有効な位置情報を取得できずに終了した場合は@(0～4)に０が設定されます。  
本コマンド実行によってUART2のPA22とPA23が有効になり、コマンド終了時に各ポートはHi-Zになります。  
GPSデバイスには標準的なNMEAフォーマットを出力するものを接続してください。（ただし、GPS機種や機器の設定によっては対応できないものもあります。）

```
例： Ugps 10000, 14 ← タイムアウ=10秒、HDOP閾値=1.4  
@(0) : 19743865 ←　20/08/16 12:24:25  
@(1) : 35710950 ←　北緯35度42.6570分 (35.710950度)  
@(2) : 139707341 ←　東経139度42.4405分 (139.707341度)  
@(3) : 161 ←　16.1M  
@(4) : 15 ←　HDOP 1.5
```

> #### ※HDOPについて  
> DOP（dilution of
> precision：精度低下率）には、PDOP、HDOP、VDOPがありますが、ここではHDOPのみを扱います。  
> HDOPは水平精度低下率で、水平方向の位置精度の低下率を数値で表し、数値が小さいほどその測位結果の精度が高いことを意味します。上空に満遍なく衛星が分布するときにHDOPは小さくなります。一般的にHDOPが1.0以下のときは高精度とみなされます。

> #### ※測位にかかる時間  
> GPSのコールドスタート時やHDOP閾値を小さな値（高精度）に設定すると測位に時間がかかります。  
> これらは、GPSデバイスの設置環境、エフェメリスやアルマナックのバックアップ状態が大きく影響します。  
> 状況に応じてHDOP閾値やタイムアウト値を調整してください。

> #### ※シリアル秒  
> GPSの測位情報から設定されるシリアル秒がUTC（協定世界時）であることに注意してください。  
> JST （日本標準時） = UTC + 32400（秒）

## Datetime

書式：Datetime \[式１\], \[式２\]  
シリアル秒の値を日付(年、月、日)、時刻（時、分、秒）に変換します。  
式１にはシリアル秒の値を指定します。（シリアル秒は、「2020/1/1,00:00:00」からの経過秒数です。）  
式２には変換後の値を格納する配列変数の先頭インデックス値（0～249）を指定します。

以下のように式２で指定されたインデックス値から６個分の配列変数に格納されます。

| 変換先   | 　変換後の値             |
|----------|--------------------------|
| ＠(＋０) | 年 (20～88) 西暦の下二桁 |
| ＠(＋１) | 月 (1～12)               |
| ＠(＋２) | 日 (1～31)               |
| ＠(＋３) | 時 (0～23)               |
| ＠(＋４) | 分 (0～59)               |
| ＠(＋５) | 秒 (0～59)               |

（「変換先」配列変数のカッコ内の数値は、引数２からのオフセット値を表します。）  
シリアル秒は、0(20/01/01 00:00:00)～2147483647（88/19 03:14:05）
が有効です。  
それ以外の値を指定してもエラーにはなりませんが、変換結果は不正な値となります。  
日付と時刻からシリアル秒を求めるには、TimeVal関数を使用することができます。

例 ：
```
 Datetime @(0)+32400,4   ←  UTCからJST(+9h)の日時に変換して@(4～9)に設定
 ```
Datetimeコマンドを文字列中で使用すると、シリアル秒の文字列変換関数として機能します。


## Distance

書式：Distance \[式１\]

GPSで計測した2地点間の距離と角度を求めます。

式1には本コマンドで使用する配列変数の先頭インデックス値（0～250）を指定します。  
式1が省略された場合は０が指定されたとみなされます。

配列変数に２地点の位置情報（緯度/経度）を設定し、演算結果はそれに続く配列変数に格納されます。

以下のように式1で指定されたインデックス値から６個分の配列変数を使用します。

<table style="width:85%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 18%" />
<col style="width: 52%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">配列変数</th>
<th colspan="2">内 容</th>
</tr>
</thead>
<tbody>
<tr>
<td>＠(＋０)</td>
<td rowspan="2">地点１の位置情報</td>
<td>緯度 (度を1000000 倍した値)、南緯は負数</td>
</tr>
<tr>
<td>＠(＋１)</td>
<td>経度 (度を1000000 倍した値)、西経は負数</td>
</tr>
<tr>
<td>＠(＋２)</td>
<td rowspan="2">地点２の位置情報</td>
<td>緯度 (度を1000000 倍した値)、南緯は負数</td>
</tr>
<tr>
<td>＠(＋３)</td>
<td>経度 (度を1000000 倍した値)、西経は負数</td>
</tr>
<tr>
<td>＠(＋４)</td>
<td rowspan="2">演算結果</td>
<td>地点２の距離 (単位メートル)</td>
</tr>
<tr>
<td>＠(＋５)</td>
<td>地点２の角度 (度を1000000 倍した値)、真北は０</td>
</tr>
</tbody>
</table>

```
>>10 @(1)=35654400
>>20 @(2)=139744770
>>30 @(3)=21422500
>>40 @(4)=39826100
>>50 Distance 1
>>60 Print @(5),@(6)
>>run
9491280　　-66947015
OK
```

---

# I2C 制御

デバイスのI2Cポートを制御するコマンドおよび変数があります。  
評価ボードではこのI2CポートにLCDとBME280が接続されていて、それぞれ専用のコマンドが用意されていますが、本章のコマンドと変数を利用することで他のI2Cデバイスとの通信が可能となります。

## I2Cコマンド

### I2cR

書式：I2cR \[式１\] , \[式２\] , \[式３\]

I2CデバイスからReadします  
式１には、I2Cのデバイスアドレスを ０～127
(\$7F)で指定します。（省略できません。）  
式２には、レジスタ（メモリアドレス）を -1～255で指定します。
（省略時は-1となります。）

-1の場合はレジスタ指定をせずRead動作のみを実行します。

式３には、Readするデータ長を0～32で指定します。（省略時は1となります。）  
I2CデバイスからReadされたデータはI2Cバッファ　に格納されます。このI2CバッファはI2cD配列変数から操作ができます。（I2cDバッファを参照）

### I2cW

書式：I2cW \[式１\] , \[式２\] , \[式３\]

I2CデバイスにＷｒｉｔｅします  
式１には、I2Cのデバイスアドレスを ０～127 (\$7F)
で指定します。（省略できません。）  
式２には、レジスタ（メモリアドレス）を -1～255で指定します。
（省略時は-1となります。）

-1の場合はレジスタ指定をせずWrite動作のみを実行します。

式３には、Writeするデータ長を0～32で指定します。（省略時は1となります。）  
I2CデバイスにWriteするデータはI2Cバッファに格納されている内容です。このI2CバッファはI2cD配列変数から操作ができます。（I2cDバッファを参照）

## I2Cバッファ

I2C制御コマンドのI2cR,
I2cWではデータの送受信に専用のI2Cバッファを使用します。  
このI2CバッファはI2cD配列変数として操作することができます。  
I2Cバッファのサイズは32バイトで、送信と受信で共通に使用されます。

### I2cD

I2Cバッファの設定と参照

書式：I2cD(式) または I2cD

式にはI2Cバッファのオフセット値（０～３１）を指定します。  
式に「－１」を指定するとボーレート設定/参照となります。（「I2Cボーレート設定」を参照してください。）  
オフセット値はI2Cバッファの先頭からのByte単位です。この変数で設定/参照する値は符号なし８ビットです。  
変数への設定値が8bit(1byte)を超えるときは下位の8bitが設定されます。  
括弧で式が指定されない場合は、I2Cバッファを文字列として設定します。（文字列としての参照はできません。）  
以下の様に、コードを指定した文字列の設定も可能です。

```
>I2cR \$76,\$D0  
OK  
>? Form("x",I2cD(0))  
60  
OK  
>Lclr  
OK  
>I2cD(0)=\$24
OK
>I2cW \$3E,\$80,1
OK
>I2cD=\$"C041C042C0434044"
OK
>I2cW \$3E,-1,8
OK
```

## I2Cのボーレート設定

I2cD変数の添え字に「－１」を指定すると、I2Cのボーレートを設定/参照することができます。

ボーレートに設定できる値は0～400 (単位はkbps)です。

400以上を指定したときは、最大値の「400 (kbps)」となります。  
0を指定したときは、デフォルトの「100 (kbps)」となります。  
I2Cのボーレートは、接続するデバイスに合わせてください。接続デバイスに適合しないボーレートを指定すると、I2Cのアクセス時にタイムアウト等のエラーになることがあります。  
起動時のボーレートはデフォルトの「100(kbps)」です。（FLASHには保存されません。）

```
>? I2cD(-1)  
100  
OK  
>Lclr  
OK  
>I2D(-1)=400  
OK  
>Lprint ”1234”  
OK
```

---

#  SPI 制御

デバイスの特定のピンをSPIポートとして制御する関数と変数があります。  
デバイスはマスターとしての動作となります。（スレーブ動作はできません。）  
下表にあるポートがSPIとして利用可能です。

| SPI信号 | ポート | 方向 | 説明                             |
|---------|--------|------|----------------------------------|
| MISO    | PB02   | 入力 | デバイスからのデバイスへのデータ入力 |
| MOSI    | PB22   | 出力 | デバイスからデバイスへのデータ出力   |
| SCLK    | PB23   | 出力 | デバイスからデバイスへのクロック出力 |

デバイス選択のSS(Slave
Select)信号は、Outpコマンド等を使用したユーザーによるポート制御が別途に必要です。  
（SSのポートはユーザーによって適切なピンに割り当ててください。）  
SPI機能の有効化および転送速度とSPIモードは、SpiBaud変数への設定によって指定します。  
SPI機能が有効のときにSPIのポートにOutpコマンドやInp関数を使用すると、そのポートのSPIポートとしての機能は無効となります。（その他のSPIポートのSPI機能は有効です。）

## SPI変数

### SpiBaud

SPIのボーレートを設定して、GPIOポートでのSPI機能を有効にします。  
SpiTRx関数を使用するにはSpiBaud変数への設定によるSPI制御の開始が必要です。  
SpiBaud変数にはボーレートをHz単位で1000～8000000の範囲で設定します。  
（下位３ビットはSPIモードとビット順の指定に使用されるので、ボーレートの値としては無視されます。）  
また、０を設定するとSPI制御を終了してGPIOを解放します。  
SpiBaud変数の下位２ビット(bit
0,1)でSPIモードを指定します。（下表を参照してください。）

<table style="width:94%;">
<colgroup>
<col style="width: 8%" />
<col style="width: 8%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 13%" />
<col style="width: 48%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">下位<br />
２ビット</th>
<th style="text-align: center;">SPI<br />
モード</th>
<th style="text-align: center;">CPOL</th>
<th style="text-align: center;">CPHA</th>
<th style="text-align: center;">アイドル時の<br />
クロック極性</th>
<th
style="text-align: center;">データのサンプリング／シフトに使われるクロックの位相</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">00</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">Low</td>
<td style="text-align: center;">Up Edgeでデータをサンプリング、Down
Edgeでシフト</td>
</tr>
<tr>
<td style="text-align: center;">01</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">Low</td>
<td style="text-align: center;">Up Edgeでシフト、Down
Edgeでデータをサンプリング</td>
</tr>
<tr>
<td style="text-align: center;">10</td>
<td style="text-align: center;">2</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">High</td>
<td style="text-align: center;">Down Edgeでデータをサンプリング、Up
Edgeでシフト</td>
</tr>
<tr>
<td style="text-align: center;">11</td>
<td style="text-align: center;">3</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">High</td>
<td style="text-align: center;">Down Edgeでシフト、Up
Edgeでデータをサンプリング</td>
</tr>
</tbody>
</table>

SpiBaud変数の下位３ビット目(bit
2)でビット順を指定します。（下表を参照してください。）

| ３ビット目 | ビット順                       |
|:----------:|--------------------------------|
|     0      | MSB（上位ビット）から送信/受信 |
|     1      | LSB（下位ビット）から送信/受信 |

SpiBaud変数はFLASHに保存されないので、リセットや電源投入毎に再設定が必要です。  
Sleep、Deepコマンド実行時にSPI制御は自動で終了しません。SPI制御が有効の時は動作していなくても僅かな電力を消費するので、省電力のためにはユーザーによるSPI制御の終了処理（SpiBaud=0）を推奨します。  
SpiBaud変数を参照すると現在の設定値が取得されます。（SPI制御が無効のときは０になります。）

## SPI関数

### SpiTRx

書式 ： SpiTRx (式１\[ ，式２\] )

式１の値をSPIポートに出力し、SPIポートから入力した値を返します。  
式２には入出力するバイト数を「１～４」で指定します。式２は省略可能で、省略または１以下の時は１、４以上のときは４となります。バイト数が１は下位８ビット、２は下位１６ビット、３は下位２４ビット、４は全３２ビットです。  
SPIポートにはリトルエンディアン（下位バイトが先頭）でデータを入出力します。  
※エンディアンの変換にはBswap16、Bswap32関数が利用できます。  
本関数を使用する前に、SpiBaud変数を設定してSPI制御を有効にする必要があります。（SPIが無効のときにSpiTRx関数を使うと、”Device
access error” になります。）

```
>Outp 28,1  
Ok  
>SpiBaud=1000000  
Ok  
>Outp 28,0:A=SpiTRx(-1,4):Outp 28,1  
Ok  
>? \$(Bswap32(A))  
12345678  
Ok  
>SpiBaud=0  
Ok
```
