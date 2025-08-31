#!/bin/bash

PP_PREFIX="make.dir/pre_proc"
mkdir -p "$PP_PREFIX"

SRC_LINE_FN_TMPL="event_fn(%s);"
SRC_LINE_ARG_TMPL="va_arg(ap, %s), "

IN_FILE="$1"
IN_POINT="$2"
OUT_FILE=${3:-"$PP_PREFIX/$1.i"}

if [[ -z $3 ]]; then 
  OUT_FILE=$(echo "$OUT_FILE" | sed 's/src/build/')
fi

mkdir -p $(dirname "$OUT_FILE")
REGEX_START=$4
REGEX_END=$5

RAW=$(clang -E $IN_FILE -o - | grep -A1 --group-separator="\n" "# $IN_POINT \"$IN_FILE\"" | sed "2p;d" | sed "s/;/;\n/g")

OLD_IFS=$IFS
IFS=$'\n'

rm -f $OUT_FILE
touch $OUT_FILE

for LINE in $(cat $IN_FILE); do
  if [[ "$LINE" =~ $REGEX_START ]]; then
    break
  fi
  echo "$LINE" >> $OUT_FILE
done

for LINE in $RAW; do
  # const char *args = "int, uint, float, double, string_t";
  if [[ "$LINE" =~ "const char *args = " ]]; then
    ARGS=$(echo "$LINE" | sed 's/.*= "//;s/";//')
    IFS=', ' read -r -a ARGS_ARR <<< "$ARGS"
    ARGS_COUNT=${#ARGS_ARR[@]}
    SRC_LINE_ARGS=""
    for ((i=0; i<ARGS_COUNT; i++)); do
      ARG_TYPE=${ARGS_ARR[$i]}
      if [[ "$ARG_TYPE" == "float" ]]; then
        ARG_TYPE="double"
      fi
      SRC_LINE_ARGS+=$(printf "$SRC_LINE_ARG_TMPL" "$ARG_TYPE")
    done
    SRC_LINE_ARGS=${SRC_LINE_ARGS%, } # remove trailing comma and space
    ARGS_DECL=$(printf "$SRC_LINE_FN_TMPL" "$SRC_LINE_ARGS")
    LINE="$ARGS_DECL"
  fi
  echo "$LINE" >> $OUT_FILE
done

TMP_FILE=$(mktemp)
for LINE in $(tac $IN_FILE); do
  if [[ "$LINE" =~ $REGEX_END ]]; then
    break
  fi
  echo "$LINE" >> $TMP_FILE
done
for LINE in $(tac $TMP_FILE); do
  echo "$LINE" >> $OUT_FILE
done
IFS=$OLD_IFS

echo "$OUT_FILE"
# Vim: set expandtab tabstop=2 shiftwidth=2:
