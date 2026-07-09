# Docker Commands

## Build Image

```bash
docker build -f qmk.Dockerfile -t qmk:local-latest .
```

## Run Image

```bash
docker run -it --rm -v ${PWD}/out:/out -v ${PWD}/r58iiz-keyboard-config:/local-qmk qmk:local-latest bash
```

## Sync `local-qmk` to image's fs

```bash
qmk-sync
```

## Compile keymap

```bash
qmk-c <qmk|vial> <keyboard-path> [keymap] [-- extra qmk compile args]
```
