# TODO

**setsid**

**jobs** 

**trap**

**wait**

**pushd /popd**

**split**

**lsof**

**wall**

getopts

find $(TOP) -name rpmbuild | xargs rm -rf;

# 常用代码段

## 软件版本比较

先给出原始版本：

```bash
#!/bin/bash

v1=$1
v2=$2
arrV1=(${v1//-/ })
arrV2=(${v2//-/ })
re="first less"
for i in $(seq 0 1)
do
  subStrOfV1=${arrV1[i]}
  subStrOfV2=${arrV2[i]}
  subArrOfV1=(${subStrOfV1//./ })
  subArrOfV2=(${subStrOfV2//./ })
  maxLen=${#subArrOfV1[*]}
  if [ ${#subArrOfV2[*]} -gt $maxLen ];then
    maxLen=${#subArrOfV2[*]}
  fi

  for j in $(seq 0 $((maxLen -1)))
  do
    echo "${subArrOfV1[j]} compare ${subArrOfV2[j]}"
    if [[ ${#subArrOfV1[j]} -lt ${#subArrOfV2[j]} ]];then
      break 2
    fi
    if [[ ${#subArrOfV1[j]} -gt ${#subArrOfV2[j]} || "${subArrOfV1[j]}" > "${subArrOfV2[j]}" ]];then
      re="first large"
      break 2
    fi
  done
done

echo "result: $re"
```

测试用例：

```bash
sh test.sh 3.10.0-1001.1 3.10.0-862.11.8
sh test.sh 3.10.0-1001.1 3.10.0-1001.11.8
sh test.sh 3.10.0-1001.13 3.10.0-1001.11.8
sh test.sh 3.10.0-888.13 3.10.0-1001.11.8
```

再给出一个精简的实现，关键在于sort -V提供的版本排序功能：

```bash
#!/bin/bash
currentver="3.10.0-957.56.1"
requiredver="3.10.0-957.156.1"
 if [ "$(printf '%s\n' "$requiredver" "$currentver" | sort -V | head -n1)" = "$requiredver" ]; then
     echo "first large"
else        
      echo "first less"
fi
```

