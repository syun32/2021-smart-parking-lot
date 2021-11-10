<?php
include('dbcon.php');
include('check.php');

if (is_login()) {

    if ($_SESSION['user_id'] == 'admin')
        header("Location: admin.php");
    else
        header("Location: welcome.php");
}
?>

    <!DOCTYPE html>
    <html>
    <head>
        <title>SMART PARKING LOT</title>
        <style>
            body {
                font-family: Consolas, monospace;
                font-family: 12px;
            }
        </style>
    </head>


    <body>

    <div class="parking" text-align="center">

        <h2>
            <center>SMART PARKING LOT</center>
        </h2>
        <hr>
        <form class="form-horizontal" method="POST" style="margin: 0 300px 0 300px;border: solid 1px;border-radius:4px"
              align="center">
            <div class="form-group" style="padding: 10px 10px 10px 10px;">
                <label for="user_name"> USER ID : </label>
                <input type="text" name="user_name" class="form-control" id="inputID" placeholder="아이디를 입력하세요."
                       required autocomplete="off" readonly onfocus="this.removeAttribute('readonly');"/>
            </div>
            <div class="form-group" style="padding: 10px 10px 10px 10px;">
                <label for="user_password">PASSWORD: </label>
                <input type="password" name="user_password" class="form-control" id="inputPassword"
                       placeholder="패스워드를 입력하세요."
                       required autocomplete="off" readonly onfocus="this.removeAttribute('readonly');"/>
            </div>
            <div class="checkbox">
                <label><input type="checkbox"> remember ID</label>
            </div>
            </br>
            <div class="from-group" style="padding: 10px 10px 10px 10px;">
                <button type="submit" name="login" class="btn btn-success">LOGIN</button>
                <a class="btn btn-success" href="registration.php" style="margin-left: 30px">
                    <span class="glyphicon glyphicon-user"></span>SIGN UP
                </a>
            </div>
            </br>
        </form>
    </div>
    </body>
    </html>


<?php

$login_ok = false;

if (($_SERVER['REQUEST_METHOD'] == 'POST') and isset($_POST['login'])) {
    $username = $_POST['user_name'];
    $userpassword = $_POST['user_password'];

    if (empty($username)) {
        $errMSG = "아이디를 입력하세요.";
    } else if (empty($userpassword)) {
        $errMSG = "패스워드를 입력하세요.";
    } else {


        try {

            $stmt = $con->prepare('select * from user where name=:username');

            $stmt->bindParam(':username', $username);
            $stmt->execute();

        } catch (PDOException $e) {
            die("Database error. " . $e->getMessage());
        }

        $row = $stmt->fetch();
        $password = $row['password'];


        if ($userpassword == $password) {
            $login_ok = true;
        }
    }


    if (isset($errMSG))
        echo "<script>alert('$errMSG')</script>";

    if ($login_ok) {
        session_regenerate_id();
        $_SESSION['user_id'] = $username;

        if ($username == 'admin')
            header('location:admin.php');
        else
            header('location:index.php');
        session_write_close();
    } else {
        echo "<script>alert('$username 인증 오류')</script>";
    }
}
?>