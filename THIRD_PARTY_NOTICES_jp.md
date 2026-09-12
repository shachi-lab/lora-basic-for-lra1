# 第三者ソフトウェアに関する通知

このリポジトリには、Shachi-labが開発したソフトウェアと、第三者から提供されたソフトウェアが含まれています。

## Shachi-lab製ソフトウェア

LoRa-BASICファームウェアおよび、そのBASICインタプリタの基になったnanoBASICの実装は、Shachi-lab製のソフトウェアです。nanoBASICは第三者コンポーネントではありません。

Shachi-labが著作権を持つソフトウェアおよび変更部分は、[MIT License](LICENSE.txt)で提供します。著作権はShachi-labが保持します。

## 第三者ソフトウェア

`LICENSE.txt`のMIT Licenseは、第三者ソフトウェアのライセンスを置き換えるものではありません。第三者の著作権表示、ライセンス表示およびライセンス文を保持し、各コンポーネントに適用される条件に従ってください。

このリポジトリで確認している第三者ソフトウェアは次のとおりです。

- **Microchip Technology / Atmel Software Framework（ASF）：** 主に`LoRa_Basic_SAMR35/src/ASF/`に含まれ、関連ファイルが`LoRa_Basic_SAMR35/src/config/`および`LoRa_Basic_SAMR35/src/asf.h`にもあります。適用される表示は各ソースファイルに記載されています。
- **Arm CMSIS：** ASFに同梱され、主に`LoRa_Basic_SAMR35/src/ASF/thirdparty/CMSIS/`および関連するCMSISデバイスヘッダーに含まれます。該当ファイルには、適用されるApache License 2.0の表示など、固有の通知が記載されています。
- **Semtech / STACKFORCE LoRaMac-node：** `LoRa_Basic_SAMR35/src/mac/`、`LoRa_Basic_SAMR35/src/radio/`、`LoRa_Basic_SAMR35/src/system/`などに取り込まれたソースです。適用されるRevised BSDの表示は各ソースファイルに記載されています。
- **Brian GladmanによるAES実装：** `LoRa_Basic_SAMR35/src/system/crypto/crypto_aes.c`および`.h`。各ファイルに記載された条件が適用されます。
- **Lander CasadoおよびPhilippas TsigasによるCMAC実装：** `LoRa_Basic_SAMR35/src/system/crypto/crypto_cmac.c`および`.h`。各ファイルに記載された条件が適用されます。

この文書は、確認できた第三者コンポーネントを案内するものであり、各ソースファイルに記載された著作権表示およびライセンス表示に代わるものではありません。ファイルの配置場所だけでライセンスを判断しないでください。ソースまたはバイナリを再配布するときは、適用されるライセンスが要求するすべての表示を保持し、必要に応じて同梱してください。

English version: [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)
