# Third-Party Notices

This repository contains software developed by Shachi-lab and software supplied by third parties.

## Shachi-lab software

The LoRa-BASIC firmware and the nanoBASIC implementation from which its BASIC interpreter was developed are Shachi-lab software. They are not third-party components.

Software and modifications for which Shachi-lab owns the copyright are licensed under the [MIT License](LICENSE.txt). Copyright remains with Shachi-lab.

## Third-party software

The MIT License in `LICENSE.txt` does not replace the licenses of third-party software. Third-party copyright notices, license notices and license texts must be retained, and each component must be used under its applicable terms.

Third-party software identified in this repository includes:

- **Microchip Technology / Atmel Software Framework (ASF):** primarily under `LoRa_Basic_SAMR35/src/ASF/`, with related files under `LoRa_Basic_SAMR35/src/config/` and `LoRa_Basic_SAMR35/src/asf.h`. Applicable notices are contained in the source files.
- **Arm CMSIS:** included with ASF, primarily under `LoRa_Basic_SAMR35/src/ASF/thirdparty/CMSIS/` and related CMSIS device headers. These files include their own notices, including Apache License 2.0 notices where applicable.
- **Semtech / STACKFORCE LoRaMac-node:** source incorporated under directories including `LoRa_Basic_SAMR35/src/mac/`, `LoRa_Basic_SAMR35/src/radio/` and `LoRa_Basic_SAMR35/src/system/`. The applicable Revised BSD notices are contained in the source files.
- **AES implementation by Brian Gladman:** `LoRa_Basic_SAMR35/src/system/crypto/crypto_aes.c` and `.h`, under the terms stated in those files.
- **CMAC implementation by Lander Casado and Philippas Tsigas:** `LoRa_Basic_SAMR35/src/system/crypto/crypto_cmac.c` and `.h`, under the terms stated in those files.

This document is a guide to identified third-party components and is not a substitute for the copyright and license notices in individual source files. A file's location alone does not determine its license. When redistributing source or binaries, retain and reproduce all notices required by the applicable licenses.

Japanese version: [THIRD_PARTY_NOTICES_jp.md](THIRD_PARTY_NOTICES_jp.md)
