<?php
include('head.php');

date_default_timezone_set('Asia/Seoul');

$stmt = $con->prepare('select * from parking');
$stmt->execute();

$count = $stmt->rowCount();
$stmt->setFetchMode(PDO::FETCH_ASSOC);
$avail = 0;
$avail_disabled = 0;

$i = 0;
while ($row = $stmt->fetch()) {
    $id[$i] = $row['id'];
    $state[$i] = $row['state'];
    $parktime[$i] = $row['park_time'];
    $disabled[$i] = $row['disabled'];
    if ($state[$i] == 0) {
        if ($disabled[$i] == 0) {
            $avail++;
        } else {
            $avail_disabled++;
        }
    }
    $i++;
}
?>

<!DOCTYPE html>
<html lang="ko">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
    <title>parking lot</title>
    <style>
        body {
            font-family: Consolas, monospace;
            font-family: 12px;
        }

        table {
            width: 50%;
            margin: auto;
        }

        th, td {
            padding: 10px;
            border-bottom: 1px solid #dadada;
            text-align: center;
        }

        td {
            width: 100px;
        }
    </style>
</head>
<body>
<?php
echo "주차 가능 구역 : " . $avail . "<br>";
echo "장애인 전용 : " . $avail_disabled;
?>
<br>
위치 : 인천광역시 미추홀구 인하로 100
<hr>

<table>
    <tbody>
    <tr height="200">
        <?php
        $now = new DateTime();
        for ($i=0;$i<3;$i++){
            if ($state[$i] == 0) {
                if ($disabled[$i] == 0) {
                    echo '<td bgcolor="lime">이용가능</td>';
                } else {
                    echo '<td bgcolor="lime">[장애인전용]<br>이용가능</td>';
                }
            } else {
                $dt = new DateTime($parktime[$i]);
                $diff = date_diff($dt, $now);
                $printdt = $diff->format("%d일 %h시간 %i분");
                echo '<td bgcolor="#d3d3d3">' . $printdt . "</td>";
            }
        }
        ?>
        <td>elevator</td>
    </tr>
    <tr height="40">
        <td></td>
    </tr>
    <tr height="200">
        <?php
        for ($i=3;$i<6;$i++){
            if ($state[$i] == 0) {
                if ($disabled[$i] == 0) {
                    echo '<td bgcolor="lime">이용가능</td>';
                } else {
                    echo '<td bgcolor="lime">[장애인전용]<br>이용가능</td>';
                }
            } else {
                $dt = new DateTime($parktime[$i]);
                $diff = date_diff($dt, $now);
                $printdt = $diff->format("%d일 %h시간 %i분");
                echo '<td bgcolor="#d3d3d3">' . $printdt . "</td>";
            }
        }
        ?>
    </tr>
    <tr height="40">
        <td></td>
        <td></td>
        <td></td>
        <td>entrance</td>
    </tr>
    </tbody>
</table>


<!--확인용 테이블-->
<!--<br>-->
<!--<br>-->
<!--<center>[Test Table]</center>-->
<!--<table>-->
<!--    <thead>-->
<!--    <tr bgcolor=#eeffcc>-->
<!--        <th>ID</th>-->
<!--        <th>STATE</th>-->
<!--        <th>TIME</th>-->
<!--    </tr>-->
<!--    </thead>-->
<!--    <tbody>-->
<!--    --><?php
//    for ($i = 0; $i < $count; $i++) {
//        echo "<tr><td>" . $id[$i] . "</td><td>" . $state[$i] . "</td><td>" . $parktime[$i] . "</td></tr>";
//    }
//    ?>
<!--    </tbody>-->
<!--</table>-->

</div>

</body>
</html>

