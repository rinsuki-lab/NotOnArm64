# NotOnArm64

Inno Setup を採用しているインストーラーに対して、Windows on Arm 上で動いていることを隠し、普通の x64 版 Windows であるかのように振舞います。

アプリケーション自体は Windows on Arm で動くが、インストーラーのスクリプトが無駄に x64 限定になっているせいでインストールできないアプリケーションを Windows on Arm 上でインストール可能にします (カーネルドライバーが必要、などの理由でそもそもアプリケーションが Windows on Arm で動かないものはこれを使ってインストールしても無意味です)。

## 使い方

NotOnArm64_Launcher.exe を起動して、当該インストーラーを選択してください。
もしインストーラーの選択が出ずに VCRUNTIME140.DLL がどうのとか言われたら https://aka.ms/vc14/vc_redist.arm64.exe をインストールしてください。

## Acknowledgements

* minhook: https://github.com/TsudaKageyu/minhook/blob/05c06c5bbca226b72ffb40fc0caaef33bcaf6f74/LICENSE.txt