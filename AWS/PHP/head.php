<?php
include('dbcon.php');
include('check.php');
date_default_timezone_set('Asia/Seoul');
?>

<html>
<head>
    <meta http-equiv="Content-Type" content="text/html;
    charset=UTF-8"/>
    <title>CONTAINER</title>
</head>

<body>
<nav class="navbar navbar-default navbar-static-top">
    <div class="container-fluid">
        <div class="navbar-header">
            <a class="navbar-brand"><h2 align="center">SMART PARKING LOT</h2></a>
            <ul class="nav navbar-nav">
                <?php if (isset($_SESSION['user_id'])) {
                    if ($_SESSION['user_id'] == 'admin') { ?>
                        <li>[관리자 모드]</li>
                        <li><a href="logout.php">LOGOUT</a></li>
                        <?php
                    } else {
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

                        echo "<li>사용자 : ".$_SESSION['user_id']."</li>";

                        $dt = new DateTime($ticket[0]);
                        $now = new DateTime();
                        $diff = date_diff($dt, $now);
                        $flag =  $dt->getTimestamp() - $now->getTimestamp();

                        if ($flag <= 0){
                            echo "<li>정기권 잔여 시간 : 0일</li>";
                            if ($flag < 0){
                                $stmt = $con->prepare("UPDATE user SET ticket=NULL WHERE name=:username");
                                $stmt->bindParam(':username', $_SESSION['user_id']);
                                $stmt->execute();
                            }
                        } else{
                            echo "<li>정기권 잔여 시간 : ".$diff->format("%y년 %m개월 %d일")."</li>";
                            echo "<li>정기권 만료일 : ".$dt->format("Y년 m월 d일")."</li>";
                        }

                        ?>
                        <li><a href="purchase.php">정기권 구매</a></li>
                        <li><a href="logout.php">LOGOUT</a></li>
                        <?php
                    }

                } else { ?>
                    <li><a href="login.php">Login</a></li>
                <?php } ?>
            </ul>
            <hr>
        </div>
    </div>
</nav>
