# `splits.txt`

このファイルは各モジュールのスプリットを含めています。

例：

```yaml
path/to/file.cpp:
	.text       start:0x80047E5C end:0x8004875C
	.ctors      start:0x803A54C4 end:0x803A54C8
	.data       start:0x803B1B40 end:0x803B1B60
	.bss        start:0x803DF828 end:0x803DFA8C
	.bss        start:0x8040D4AC end:0x8040D4D8 common
```

## フォーマット

```yaml
path/to/file.cpp: [file attributes]
    section     [section attributes]
    ...
```

- `path/to/file.cpp` ソースファイルのファイル名：普段からパスがsrcから始める。**ソースファイル存在の必要がない。** 
  This corresponds to an entry in `configure.py` for specifying compiler flags and other options.
  各スプリットが`configure.py`のエントリーに対応します。そこには、コンパイラーフラッグとそのような設定を特定する所です。

### セクション資質

- `start:` ファイルにセクションのスタートアドレス（例： `0x80001234`）。
- `end:` ファイルにセクションのエンドアドレス。
- `align:` セクションのアライメントを特定します。
