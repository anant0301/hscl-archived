name="ups_bench"
while true
do
    if ! pgrep -x $name > /dev/null
    then
        echo "ups_bench is not running"
    fi

done
