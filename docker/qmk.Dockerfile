ARG VIAL_UPSTREAM_ORG=vial-kb
ARG VIAL_UPSTREAM_REPO=vial-qmk
ARG VIAL_UPSTREAM_BRANCH=vial

ARG QMK_UPSTREAM_ORG=qmk
ARG QMK_UPSTREAM_REPO=qmk_firmware
ARG QMK_UPSTREAM_BRANCH=master

ARG CONFIG_GITHUB_USERNAME=r58iiz
ARG CONFIG_REPO_NAME=r58iiz-keyboard-config
ARG CONFIG_BRANCH=main

FROM debian:trixie-slim

ARG VIAL_UPSTREAM_ORG
ARG VIAL_UPSTREAM_REPO
ARG VIAL_UPSTREAM_BRANCH
ARG QMK_UPSTREAM_ORG
ARG QMK_UPSTREAM_REPO
ARG QMK_UPSTREAM_BRANCH
ARG CONFIG_GITHUB_USERNAME
ARG CONFIG_REPO_NAME
ARG CONFIG_BRANCH

ENV DEBIAN_FRONTEND=noninteractive
ENV KEEB_HOME=/keeb
ENV QMK_TREE=${KEEB_HOME}/qmk_firmware
ENV VIAL_TREE=${KEEB_HOME}/${VIAL_UPSTREAM_REPO}
ENV LOCAL_QMK_OVERLAY=/local-qmk
ENV CONFIG_REPO_URL=https://github.com/${CONFIG_GITHUB_USERNAME}/${CONFIG_REPO_NAME}
ENV CONFIG_BRANCH=${CONFIG_BRANCH}
ENV PATH="/root/.local/bin:${PATH}"

RUN apt-get update && apt-get install -y \
    dos2unix \
    jq \
    fzf \
    git \
    tar \
    vim \
    zip \
    curl \
    file \
    gzip \
    less \
    tmux \
    tree \
    wget \
    rsync \
    bzip2 \
    unzip \
    ripgrep \
    xz-utils \
    bash-completion \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR ${KEEB_HOME}

RUN curl -fsSL https://install.qmk.fm | sh -s -- \
    --confirm \
    --skip-qmk-flashutils \
    --skip-udev-rules \
    --skip-windows-drivers

RUN git clone --depth 1 -b ${QMK_UPSTREAM_BRANCH} \
    --recurse-submodules --shallow-submodules \
    https://github.com/${QMK_UPSTREAM_ORG}/${QMK_UPSTREAM_REPO} ${QMK_TREE}

RUN git clone --depth 1 -b ${VIAL_UPSTREAM_BRANCH} \
    --recurse-submodules --shallow-submodules \
    https://github.com/${VIAL_UPSTREAM_ORG}/${VIAL_UPSTREAM_REPO} ${VIAL_TREE}

COPY scripts/qmk-sync /root/.local/bin/qmk-sync
RUN dos2unix /root/.local/bin/qmk-sync
COPY scripts/qmk-c /root/.local/bin/qmk-c
RUN dos2unix /root/.local/bin/qmk-c
RUN chmod +x /root/.local/bin/qmk-sync /root/.local/bin/qmk-c

RUN printf '\n%s\n' \
  'PS1="\[\e[1;32m\]\u\[\e[1;35m\]:\[\e[1;34m\]\W \[\e[1;31m\]\$\[\e[0m\] "' \
  >> /root/.bashrc

CMD ["bash"]
