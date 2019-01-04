Code coverage
=============

This page shows the code coverage results for the scripts in the
:code:`tests/` directory using the
`gcov <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>`_ and
`lcov <http://ltp.sourceforge.net/coverage/lcov.php>`_ tools.
In a perfect world, the test scripts should hit every line of code.
If your module has low coverage, you need to write some more test
scripts!


.. raw:: html

    <!--
    *Very* low security password protection.
    Based on https://github.com/chrisssycollins/protected-github-pages.
    The hash corresponding to the correct password is stored in the $COVERAGE_HASH
    variable on TRAVIS.
    -->

    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="robots" content="noindex, nofollow">

    <style>
        *,
        *:after,
        *:before {
            box-sizing: border-box;
        }
        body,
        html {
            background: #f2f2f2;
            color: #2D3737;
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 100%;
        }

        .protected {
            background: #fff;
            -webkit-box-shadow: 0 2px 3px 0 rgba(0,0,0,0.1);
            box-shadow: 0 2px 3px 0 rgba(0,0,0,0.1);
            border-radius: 3px;
            min-width: 500px;

        }
        .protected__content {
            padding: 24px 28px;
        }
        .protected__content__heading {
            font-size: 16px;
            font-weight: 500;
            margin: 0 0 12px;
            line-height: 1;
        }
        .protected__alert {
            display: none;
            border-bottom: 1px solid transparent;
            border-radius: 3px 3px 0 0;
            padding: 12px 14px;
            color: #a94442;
            background-color: #f2dede;
            border-color: #ebccd1;
        }
        .protected__content__input {
            display: block;
            border: solid 1px #ccc;
            padding: 12px 14px;
            -webkit-box-shadow: 0 2px 3px 0 rgba(0,0,0,0.1);
            box-shadow: 0 2px 3px 0 rgba(0,0,0,0.1);
            font-size: 16px;
            width: 100%;
            border-radius: 3px;

            margin-bottom: 12px;
        }
        .protected__content__input:focus {
            outline: none;
            border-color: #228843;
        }
        .protected__content__btn {
            background-color: #228843;
            border-radius: 3px;
            cursor: pointer;
            border: none;
            color: #fff;
            padding: 12px 14px;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Roboto", "Oxygen", "Ubuntu", "Cantarell", "Fira Sans", "Droid Sans", "Helvetica Neue", sans-serif;
            font-weight: 500;
            font-size: 16px;

        }
        .protected__content__btn:hover {
            background-color: #1C6D36;
        }

    </style>

    <div class="protected">
        <div class="protected__alert" data-id="alert">You entered the wrong password</div>
        <div class="protected__content">
            <h1 class="protected__content__heading">You need a password to continue.</h1>
            <input class="protected__content__input" data-id="password" type="password" placeholder="Hint: Rory's motto"/>
            <button data-id="button" type="button" class="protected__content__btn">Continue</button>
        </div>
    </div>
    <br/><br/>

    <script type="text/javascript" src="https://rawcdn.githack.com/chrisveness/crypto/7067ee62f18c76dd4a9d372a00e647205460b62b/sha1.js"></script>
    <script type="text/javascript">
        "use strict"
        var button = document.querySelectorAll('[data-id="button"]')
        var password = document.querySelectorAll('[data-id="password"]')

        function login(secret) {
            var hash = Sha1.hash(secret)
            var url = hash + "/index.html"
            var alert = document.querySelectorAll('[data-id="alert"]')

            var request = new XMLHttpRequest()
            request.open('GET', url, true)

            request.onload = function () {
                if (request.status >= 200 && request.status < 400) {
                    window.location = url
                } else {
                    parent.location.hash = hash
                    alert[0].style.display = 'block'
                    password[0].setAttribute('placeholder', 'Incorrect password')
                    password[0].value = ''
                }
            }
            request.onerror = function () {
                parent.location.hash = hash
                alert[0].style.display = 'block'
                password[0].setAttribute('placeholder', 'Incorrect password')
                password[0].value = ''
            }
            request.send()
        }

        button[0].addEventListener("click", function () {
            login(password[0].value)
        })

        document.onkeydown = function (e) {
            e = e || window.event
            if (e.keyCode == 13) {
                login(password[0].value)
            }
        }
    </script>
