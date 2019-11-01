#!/bin/bash
ldconfig

chsh -s /bin/bash ${RUN_USER}

echo -e "${USER_PASSWORD}\n${USER_PASSWORD}" | passwd ${RUN_USER}

mkdir -p /home/${RUN_USER}/${RUN_USER}/data

chmod -R 700 "/home/${RUN_USER}/"
chown -R "${RUN_USER}:${RUN_GROUP}" "/home/${RUN_USER}/"
chown -R "${RUN_USER}:${RUN_GROUP}" "/src/"

env | grep LD_LIBRARY_PATH >> /home/${RUN_USER}/.bashrc

/usr/bin/ssh-keygen -A
mkdir -p /run/sshd
/usr/sbin/sshd -D

service ssh start

