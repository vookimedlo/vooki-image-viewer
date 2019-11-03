Build, sign and, submit an application in DMG for a notarization.

```
./generate.sh
./build.sh
./deploy.sh
./sign.sh
./package.sh
./sign-package.sh
./notarize.sh
```

Wait for a notification email from Apple.

```
./notarize-check.sh <RequestUUID>
./staple.sh
```
