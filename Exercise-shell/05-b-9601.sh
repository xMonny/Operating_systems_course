#!/bin/bash

if [ "$#" -lt 1 -o "$#" -gt 2 ]; then
	echo "Invalid number of arguments"
	exit 1
fi

BACKUP=$BACKUP_DIR
FIRST="${1}"

if [ "$#" -eq 2 ]; then
	SECOND="${2}"

	if ! [ -d "${SECOND}" ]; then
		echo "Second argument is invalid name of a directory"
		exit 2
	fi

	if ! [ -w "${SECOND}" ]; then
		echo "Directory doesn't have write permissions"
		exit 3
	fi
fi

RECOVERY_LIST=$(mktemp)

function getRecover
{
	while read file
	do
		BASE_NAME=$(basename "${file}")
		FILE_NAME=$(echo "${BASE_NAME}" | cut -d '_' -f1)
		DATE=$(echo "${BASE_NAME}" | cut -d '_' -f2 | cut -d '-' -f1,2,3 | tr '-' '/')
		TIME=$(echo "${BASE_NAME}" | cut -d '_' -f2 | cut -d '-' -f4,5,6 | tr '-' ':' | cut -d '.' -f1)

		echo "${FILE_NAME} (${DATE} ${TIME})" >> "${RECOVERY_LIST}"
	done< <(find "${BACKUP}" -type f -perm -u=r 2>/dev/null)
}

if [ "${FIRST}" = "-l" ]; then
	echo "Files that can be recovered: "

	getRecover
	cat "${RECOVERY_LIST}"
else
	getRecover
	FOUND=$(cat "${RECOVERY_LIST}" | grep "^${FIRST} ")

	if [ -z "${FOUND}" ]; then
		echo "File ${FIRST} doesn't exist in the recovery list"
		exit 4
	fi

	if [ "$#" -eq 1 ]; then
		SECOND="."
	fi

	NUMBER_FILES=$(echo "${FOUND}" | wc -l)

	if [ "${NUMBER_FILES}" -eq 1 ]; then
		FILE_RECOVER=$(find "${BACKUP}" -type f -perm -u=r 2>/dev/null | fgrep "${FIRST}_")
		BASE_NAME=$(basename "${FILE_RECOVER}" | sed -E "s/\.(gz|tgz)$//g")
		FILE_PATH=$(echo "${BACKUP}${BASE_NAME}")
		tar -xf "${FILE_RECOVER}"
		mv "${FILE_PATH}" "${SECOND}" 2>/dev/null
		rm -r "${FILE_RECOVER}"
	else
		COUNT=1
		TEMP_FILE=$(mktemp)
		while read file
		do
			MATCHED=$(echo "${file}" | grep "^${FIRST} ")
			if [ -n "${MATCHED}" ]; then
				echo "${COUNT}. ${file}" >> "${TEMP_FILE}"
				COUNT=$((COUNT+1))
			fi
		done< <(cat "${RECOVERY_LIST}")

		cat "${TEMP_FILE}"

		read -p "Choose an option: " OPTION

		CHOSEN=$(cat "${TEMP_FILE}" | egrep "^${OPTION}\. ")

		if [ -z "${CHOSEN}" ]; then
			echo "[exited]"
			exit 0
		else
			CHOSEN=$(echo "${CHOSEN}" | cut -d ' ' -f2-)
			FILE_NAME=$(echo "${CHOSEN}" | cut -d ' ' -f1)
			DATE=$(echo "${CHOSEN}" | cut -d '(' -f2 | cut -d ' ' -f1 | tr '/' '-')
			TIME=$(echo "${CHOSEN}" | cut -d '(' -f2 | cut -d ' ' -f2 | sed 's/)//g' | tr ':' '-')

			FULL_NAME=$(echo "${FILE_NAME}_${DATE}-${TIME}")

			FILE_RECOVER=$(find "${BACKUP}" -type f -perm -u=r 2>/dev/null | fgrep "${FULL_NAME}")
			BASE_NAME=$(basename "${FILE_RECOVER}" | sed -E "s/\.(gz|tgz)$//g")
			FILE_PATH=$(echo "${BACKUP}${BASE_NAME}")
			tar -xf "${FILE_RECOVER}"
			mv "${FILE_PATH}" "${SECOND}" 2>/dev/null
			rm -r "${FILE_RECOVER}"
		fi
		rm "${TEMP_FILE}"	
	fi
fi

rm "${RECOVERY_LIST}"
