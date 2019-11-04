#!/bin/bash

#!/bin/bash
set -e

REDIS_PASSWORD=${REDIS_PASSWORD:-}

map_redis_uid() {
  USERMAP_ORIG_UID=$(id -u redis)
  USERMAP_ORIG_GID=$(id -g redis)
  USERMAP_GID=${USERMAP_GID:-${USERMAP_UID:-$USERMAP_ORIG_GID}}
  USERMAP_UID=${USERMAP_UID:-$USERMAP_ORIG_UID}
  if [ "${USERMAP_UID}" != "${USERMAP_ORIG_UID}" ] || [ "${USERMAP_GID}" != "${USERMAP_ORIG_GID}" ]; then
    echo "Adapting uid and gid for redis:redis to $USERMAP_UID:$USERMAP_GID"
    groupmod -g "${USERMAP_GID}" redis
    sed -i -e "s/:${USERMAP_ORIG_UID}:${USERMAP_GID}:/:${USERMAP_UID}:${USERMAP_GID}:/" /etc/passwd
  fi
}

create_socket_dir() {
  mkdir -p /run/redis
  chmod -R 0755 /run/redis
  chown -R ${REDIS_USER}:${REDIS_USER} /run/redis
}

create_data_dir() {
  mkdir -p ${REDIS_DATA_DIR}
  chmod -R 0755 ${REDIS_DATA_DIR}
  chown -R ${REDIS_USER}:${REDIS_USER} ${REDIS_DATA_DIR}
}

create_log_dir() {
  mkdir -p ${REDIS_LOG_DIR}
  chmod -R 0755 ${REDIS_LOG_DIR}
  chown -R ${REDIS_USER}:${REDIS_USER} ${REDIS_LOG_DIR}
}

map_redis_uid
create_socket_dir
create_data_dir
create_log_dir

ldconfig

chsh -s /bin/bash ${RUN_USER}

echo -e "${USER_PASSWORD}\n${USER_PASSWORD}" | passwd ${RUN_USER}

mkdir -p /home/${RUN_USER}/${RUN_USER}/data

chmod -R 700 "/home/${RUN_USER}/"
chown -R "${RUN_USER}:${RUN_GROUP}" "/home/${RUN_USER}/"
# chown -R "${RUN_USER}:${RUN_GROUP}" "/src/"

env | grep LD_LIBRARY_PATH >> /home/${RUN_USER}/.bashrc

/usr/bin/ssh-keygen -A
mkdir -p /run/sshd

echo "starting sshd"
/usr/sbin/sshd -D &

echo "starting ssh service"
service ssh start

echo "starting redis service"
exec speedus redis-server /etc/redis/redis.conf
