ARG BASE_IMAGE=debian:trixie-slim@sha256:d7e12182ce18b85b93007c1dedf31f2d29e01ccf3182cc4017c709b6259bc132

ARG VIAL_UPSTREAM_ORG=vial-kb
ARG VIAL_UPSTREAM_REPO=vial-qmk
ARG VIAL_UPSTREAM_REF=dd43959ae5c08d8a28d38a1acf7b04e86b14a344

ARG QMK_UPSTREAM_ORG=qmk
ARG QMK_UPSTREAM_REPO=qmk_firmware
ARG QMK_UPSTREAM_TAG=0.34.4

ARG CONFIG_GITHUB_USERNAME=r58iiz
ARG CONFIG_REPO_NAME=keyboard-config
ARG CONFIG_BRANCH=main

# ---- QMK clone stage ----
FROM ${BASE_IMAGE} AS qmk-clone
ARG QMK_UPSTREAM_ORG
ARG QMK_UPSTREAM_REPO
ARG QMK_UPSTREAM_TAG
RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates git \
    && rm -rf /var/lib/apt/lists/*
RUN git init /qmk_firmware \
    && git -C /qmk_firmware remote add origin \
    https://github.com/${QMK_UPSTREAM_ORG}/${QMK_UPSTREAM_REPO} \
    && git -C /qmk_firmware fetch --depth 1 origin ${QMK_UPSTREAM_TAG} \
    && git -C /qmk_firmware checkout --detach FETCH_HEAD \
    && git -C /qmk_firmware submodule update --init --recursive --depth 1

# ---- Vial clone stage ----
FROM ${BASE_IMAGE} AS vial-clone
ARG VIAL_UPSTREAM_ORG
ARG VIAL_UPSTREAM_REPO
ARG VIAL_UPSTREAM_REF
RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates git \
    && rm -rf /var/lib/apt/lists/*
RUN git init /vial-qmk \
    && git -C /vial-qmk remote add origin \
    https://github.com/${VIAL_UPSTREAM_ORG}/${VIAL_UPSTREAM_REPO} \
    && git -C /vial-qmk fetch --depth 1 --no-tags origin ${VIAL_UPSTREAM_REF} \
    && git -C /vial-qmk checkout --detach FETCH_HEAD \
    && git -C /vial-qmk submodule update --init --recursive --depth 1

# ---- Final image ----
FROM ${BASE_IMAGE}
ARG VIAL_UPSTREAM_REPO
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
    python3-pip \
    python3-yaml \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install --break-system-packages keymap-drawer==0.23.0

WORKDIR ${KEEB_HOME}

RUN curl -fsSL https://install.qmk.fm | sh -s -- \
    --confirm \
    --skip-qmk-flashutils \
    --skip-udev-rules \
    --skip-windows-drivers

COPY --from=qmk-clone /qmk_firmware ${QMK_TREE}
COPY --from=vial-clone /vial-qmk ${VIAL_TREE}

COPY scripts/ /root/.local/bin/
RUN find /root/.local/bin/ -type f -exec dos2unix {} + \
    && chmod +x /root/.local/bin/*

RUN printf '\n%s\n' \
    'PS1="\[\e[1;32m\]\u\[\e[1;35m\]:\[\e[1;34m\]\W \[\e[1;31m\]\$\[\e[0m\] "' \
    >> /root/.bashrc

CMD ["bash"]
