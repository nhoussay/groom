(note: this is a temporary file, to be added-to by anybody, and moved to release-notes at release time)

GROOM Core version *version* is now available from:  <https://github.com/groom-project/groom/releases>

This is a new major version release, including various bug fixes and performance improvements, as well as updated translations.

Please report bugs using the issue tracker at github: <https://github.com/groom-project/groom/issues>


How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then run the installer (on Windows) or just copy over /Applications/GROOM-Qt (on Mac) or groomd/groom-qt (on Linux).

Sapling Parameters
==================

In order to run, GROOM Core now requires two files, `sapling-output.params` and `sapling-spend.params` (with total size ~50 MB), to be saved in a specific location.

For the following packages, no action is required by the user:
- macOS release `dmg` binaries will use the params that are bundled into the .app bundle.
- Windows installer `.exe` will automatically copy the files in the proper location.
- Linux `PPA/Snap` installs will automatically copy the files in the proper location.

For the other packages, the user must save the param files in the proper location, before being able to run GROOM v5.0.0:
- macOS/Linux `tar.gz` tarballs include a bash script (`install-params.sh`) to copy the parameters in the appropriate location.
- Windows `.zip` users need to manually copy the files from the `share/groom` folder to the `%APPDATA%\groomParams` directory.
- self compilers can run the script from the repository sources (`params/install-params.sh`), or copy the files directly from the `params` subdirectory.

Compatibility
==============

GROOM Core is extensively tested on multiple operating systems using the Linux kernel, macOS 10.10+, and Windows 7 and later.

Microsoft ended support for Windows XP on [April 8th, 2014](https://www.microsoft.com/en-us/WindowsForBusiness/end-of-xp-support), No attempt is made to prevent installing or running the software on Windows XP, you can still do so at your own risk but be aware that there are known instabilities and issues. Please do not report issues about Windows XP to the issue tracker.

Apple released it's last Mountain Lion update August 13, 2015, and officially ended support on [December 14, 2015](http://news.fnal.gov/2015/10/mac-os-x-mountain-lion-10-8-end-of-life-december-14/). GROOM Core software starting with v3.2.0 will no longer run on MacOS versions prior to Yosemite (10.10). Please do not report issues about MacOS versions prior to Yosemite to the issue tracker.

GROOM Core should also work on most other Unix-like systems but is not frequently tested on them.


Notable Changes
==============

(Developers: add your notes here as part of your pull requests whenever possible)

Cold-Staking Re-Activation
--------------------------
GROOM Core v6.0.0 includes a fix for the vulnerability identified within the cold-staking protocol (see PR [#2258](https://github.com/nhoussay/groom/pull/2258)).
Therefore the feature will be re-enabled on the network, via `SPORK_19`, shortly after the upgrade enforcement.

#### Protocol changes

A new opcode (`0xd2`) is introduced (see PR [#2275](https://github.com/nhoussay/groom/pull/2275)). It enforces the same rules as the legacy cold-staking opcode, but without allowing a "free" script for the last output of the transaction.
This is in accord with the consensus change introduced with the "Deterministic Masternodes" update, as masternode/budget payments are now outputs of the *coinbase* transaction (rather than the *coinstake*), therefore a "free" output for the coinstake is no longer needed.
The new opcode takes the name of `OP_CHECKCOLDSTAKEVERIFY`, and the legacy opcode (`0xd1`) is renamed to `OP_CHECKCOLDSTAKEVERIFY_LOF` (last-output-free).
Scripts with the old opcode are still accepted on the network (the restriction on the last-output is enforced after the script validation in this case), but the client creates new delegations with the new opcode, by default, after the upgrade enforcement.


GUI changes
-----------

### RPC-Console

The GUI RPC-Console now accepts "parenthesized syntax", nested commands, and simple queries (see [PR #2282](https://github.com/nhoussay/groom/pull/2282).
A new command `help-console` (available only on the GUI console) documents how to use it:

```
This console accepts RPC commands using the standard syntax.
    example:    getblockhash 0

This console can also accept RPC commands using parenthesized syntax.
    example:    getblockhash(0)

Commands may be nested when specified with the parenthesized syntax.
    example:    getblock(getblockhash(0) true)

A space or a comma can be used to delimit arguments for either syntax.
    example:    getblockhash 0
                getblockhash,0

Named results can be queried with a non-quoted key string in brackets.
    example:    getblock(getblockhash(0) true)[tx]

Results without keys can be queried using an integer in brackets.
    example:    getblock(getblockhash(0),true)[tx][0]
```

#### Allow to optional specify the directory for the blocks storage

A new init option flag '-blocksdir' will allow one to keep the blockfiles external from the data directory.


#### Show wallet's auto-combine settings in getwalletinfo

`getwalletinfo` now has two additional return fields. `autocombine_enabled` (boolean) and `autocombine_threshold` (numeric) that will show the auto-combine threshold and whether or not it is currently enabled.

#### Deprecate the autocombine RPC command

The `autocombine` RPC command has been replaced with specific set/get commands (`setautocombinethreshold` and `getautocombinethreshold`, respectively) to bring this functionality further in-line with our RPC standards. Previously, the `autocombine` command gave no user-facing feedback when the setting was changed. This is now resolved with the introduction of the two new commands as detailed below:

* `setautocombinethreshold`
    ```  
    setautocombinethreshold enable ( value )
    This will set the auto-combine threshold value.
    Wallet will automatically monitor for any coins with value below the threshold amount, and combine them if they reside with the same GROOM address
    When auto-combine runs it will create a transaction, and therefore will be subject to transaction fees.
    
    Arguments:
    1. enable          (boolean, required) Enable auto combine (true) or disable (false)
    2. threshold       (numeric, optional. required if enable is true) Threshold amount. Must be greater than 1.
    
    Result:
    {
      "enabled": true|false,     (boolean) true if auto-combine is enabled, otherwise false
      "threshold": n.nnn,        (numeric) auto-combine threshold in PIV
      "saved": true|false        (boolean) true if setting was saved to the database, otherwise false
    }
    ```

* `getautocombinethreshold`
    ```
    getautocombinethreshold
    Returns the current threshold for auto combining UTXOs, if any

    Result:
    {
      "enabled": true|false,    (boolean) true if auto-combine is enabled, otherwise false
      "threshold": n.nnn         (numeric) the auto-combine threshold amount in PIV
    }
    ```

#### Build system changes

The minimum supported miniUPnPc API version is set to 10. This keeps compatibility with Ubuntu 16.04 LTS and Debian 8 `libminiupnpc-dev` packages. Please note, on Debian this package is still vulnerable to [CVE-2017-8798](https://security-tracker.debian.org/tracker/CVE-2017-8798) (in jessie only) and [CVE-2017-1000494](https://security-tracker.debian.org/tracker/CVE-2017-1000494) (both in jessie and in stretch).


#### Build System

OpenSSL is no longer used by GROOM Core


#### Disable PoW mining RPC Commands

A new configure flag has been introduced to allow more granular control over weather or not the PoW mining RPC commands are compiled into the wallet. By default they are not. This behavior can be overridden by passing `--enable-mining-rpc` to the `configure` script.

#### Removed startup options

- `printstakemodifier`

Configuration sections for testnet and regtest
----------------------------------------------

It is now possible for a single configuration file to set different options for different networks. This is done by using sections or by prefixing the option with the network, such as:

    main.uacomment=groom
    test.uacomment=groom-testnet
    regtest.uacomment=regtest
    [main]
    mempoolsize=300
    [test]
    mempoolsize=100
    [regtest]
    mempoolsize=20

The `addnode=`, `connect=`, `port=`, `bind=`, `rpcport=`, `rpcbind=`, and `wallet=` options will only apply to mainnet when specified in the configuration file, unless a network is specified.


#### Logging

The log timestamp format is now ISO 8601 (e.g. "2021-02-28T12:34:56Z").


#### Automatic Backup File Naming

The file extension applied to automatic backups is now in ISO 8601 basic notation (e.g. "20210228T123456Z"). The basic notation is used to prevent illegal `:` characters from appearing in the filename.

*version* Change log
==============

Detailed release notes follow. This overview includes changes that affect behavior, not code moves, refactors and string updates. For convenience in locating the code changes and accompanying discussion, both the pull request and git merge commit are mentioned.

### GUI

### Wallet

### RPC

### Masternodes/Budget

### Core

### Build Systems

### P2P/Network

### Testing

### Cleanup/Refactoring

### Docs/Output

## Credits

Thanks to everyone who directly contributed to this release:


As well as everyone that helped translating on [Transifex](https://www.transifex.com/projects/p/groom-project-translations/), the QA team during Testing and the Node hosts supporting our Testnet.
