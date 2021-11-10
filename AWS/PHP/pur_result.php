<?php
include('dbcon.php');
include('check.php');
date_default_timezone_set('Asia/Seoul');

$stmt = $con->prepare("select ticket from user where name=:username");
$stmt->bindParam(':username', $_SESSION['user_id']);
$stmt->execute();
$count = $stmt->rowCount();
$stmt->setFetchMode(PDO::FETCH_ASSOC);

$i = 0;
while ($row = $stmt->fetch()) {
    $ticket[$i] = $row['ticket'];
    $i++;
}


echo $ticket[0]."<br>";

$purch = $_GET["purch"];
echo $purch . "<br>";

if (isset($ticket[0])) {
    $dt = strtotime($ticket[0]);
    $res = date("Y-m-d 00:00:00", $dt + $purch);
} else {
    $now = time();
    $res = date("Y-m-d 00:00:00", $now + $purch);
}

echo $res;


$stmt = $con->prepare("UPDATE user SET ticket=:res WHERE name=:username");
$stmt->bindParam(':res', $res);
$stmt->bindParam(':username', $_SESSION['user_id']);


if ($stmt->execute()) {
    echo "<script>alert('구매 완료')</script>";
} else {
    echo "<script>alert('구매 실패 : 관리자에게 문의하세요')</script>";
}

header("Location: index.php");
?>
<!--<br>-->
<!--<a href="index.php">GO HOME</a>-->