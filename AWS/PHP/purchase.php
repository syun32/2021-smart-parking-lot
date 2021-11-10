<?php
include('head.php');
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
        <center>정기권 구매</center>
    </h2>
    <hr>
    <form name="purch" method="get" action="pur_result.php"
          style="margin: 0 300px 0 300px;border: solid 1px;border-radius:4px">
        <br>
        <input type="radio" name="purch" value=604800>7일<br>
        <input type="radio" name="purch" value=1209600>14일<br>
        <input type="radio" name="purch" value=2629800>1개월<br>
        <input type="radio" name="purch" value=7889400>3개월<br>
        <input type="radio" name="purch" value=15778800>6개월<br>
        <input type="radio" name="purch" value=31557600>1년<br><br>
        <input type="submit" value="결제">
        <br><br>
    </form>
</div>
</body>
</html>
