# Практика 1. Инструменты разработки ПО

[![Feedback](feedback.png)][feedback_day1]

## Цели

__Цель данной работы__ — ознакомиться с современным рабочим процессом (workflow)
и используемыми в нем инструментами. Так, будут рассмотрены следующие
инструменты:

  - [Git][git] как пример системы контроля версий (VCS).
  - [GitHub][github] как пример хостинга для проектов.
  - [Google Test][gtest] как пример фреймфорка для модульного тестирования
    (xUnit).
  - [CMake](http://www.cmake.org) как кроссплатформенная и IDE-независимая
    система сборки для проектов на языке С/С++.

Кроме того, в процессе выполнения задания нужно будет ознакомиться с общей
структурой проекта, используемыми функциями OpenCV и теми функциями, которые
необходимо будет оптимизировать на протяжении школы.

## Задачи

__Основные задачи__

  1. __Git__. Ознакомиться с основными операциями Git: клонирование, создание
     веток, внесение изменений, работа с удаленными репозиториями.
  1. __GitHub + Travis-CI__. Познакомиться с базовыми операциями GitHub в связке
     с Travis-CI: создание форка, создание pull request, контроль корректности
     на Travis-CI.
  1. __CMake + MSVS__. Научиться строить проект, реализованный с использованием
     CMake. Освоить основные операции с кодом: запуск демо-приложения и сборок с
     тестами.
  1. __Google Test__. Научиться создавать модульные тесты при помощи Google
     Test. К реализации предлагаются следующие тесты:
     1. Для функций `ConvertColor_BGR2GRAY_BT709` и `GuoHallThinning` нужно
        проверить, что выходная картинка имеет такие же размеры, как и исходная.
     1. Для функции `ImageResize` нужно проверить, что выходное изображение
        имеет тот размер, который был передан в качестве аргумента.
     1. Необходимо проверить, что если на вход функции `ImageResize` подать
        одноцветное изображение, то на выходе тоже будет изображение, все
        пикселы которого имеют то же самое значение.
     1. Необходимо проверить, что если на вход функции
        `ConvertColor_BGR2GRAY_BT709` подать трехканальное одноцветное
        изображение, то на выходе тоже будет изображение, все пикселы которого
        имеют одинаковые значения.
     1. Необходимо проверить, что на выходном изображении из функции
        `GuoHallThinning` черных пикселов не меньше, чем на входном.

__Дополнительные задачи__

  - Ознакомиться с реализацией функций `ConvertColor_BGR2GRAY_BT709`,
    `ImageResize`, `GuoHallThinning`.
  - Добавить еще несколько тестов, имеющих по вашему мнению смысл.
  - Одна из функций содержит ошибку. Реализуйте тест для идентификации данной ошибки и исправьте её.

## Общая последовательность действий

  1. Сделать форк [upstream][upstream]-репозитория и клонировать
     [origin][origin]-репозиторий к себе на локальную машину.
  1. Собрать проект и проверить его работоспособность, запустив тесты и
     демо-приложение. Отдельно стоит запустить приложение с опцией `--save`, и
     проанализировать сохраненные изображения, демонстрирующие пошаговую работу
     алгоритма.
  1. Создать новую ветку в Git для выполнения практического задания. Реализовать
     фиктивный тест, прислать первый pull request и ознакомиться с отчетом о
     тестировании на системе непрерывной интеграции [Travis-CI][travis].
  1. Реализовать несколько модульных тестов. После реализации каждого теста
     следует пересобирать проект и проверять работоспособность тестов. Изменения
     необходимо постоянно фиксировать в истории Git, выкладывая в рабочую ветку
     на GitHub и контролируя успешность тестирования на Travis-CI.
  1. Если останется время, можно заняться выполнением дополнительных задач.

## Общие инструкции по работе с Git

### Первичная настройка

В данном разделе описана типичная последовательность действий, которую
необходимо выполнить перед тем, как начать работать с проектом.

  1. Создать свой личный аккаунт на [GitHub][github], если таковой
     отсутствует. Для определенности обозначим аккаунт `github-account`.

  1. Сделать fork [центрального репозитория][upstream] (в терминологии Git
     upstream-репозиторий) к себе в личное пространство на GitHub. В результате
     будет создана копия репозитория с названием
     <https://github.com/github-account/itseez-ws-2016-practice>
     (origin-репозиторий).

  1. Клонировать origin-репозиторий к себе на локальный компьютер,
     воспользовавшись следующей командой:

  ```bash
  $ git clone https://github.com/github-account/itseez-ws-2016-practice
  ```

  1. Перейти в директорию `itseez-ws-2016-practice`:

  ```bash
  $ cd ./itseez-ws-2016-practice
  ```

  1. Настроить адрес upstream-репозитория (потребуется для публикации
     изменений):

  ```bash
  $ git remote add upstream https://github.com/itseez-academy/itseez-ws-2016-practice
  ```

  1. Настроить имя пользователя, из-под которого будут выполняться все операции
     с репозиторием Git:

  ```bash
  $ git config --local user.name "Your Name"
  $ git config --local user.email "your.email@somewhere.com"
  ```

  Примечание: если не выполнить указанную операцию, то при попытке размещения
  изменений на сервер потребуется вводить ваш `github-account` (появится
  сообщение, приведенное ниже).

  ```
  warning: push.default is unset; its implicit value is changing in
  Git 2.0 from 'matching' to 'simple'. To squelch this message
  and maintain the current behavior after the default changes, use:

    git config --global push.default matching

  To squelch this message and adopt the new behavior now, use:

    git config --global push.default simple

  When push.default is set to 'matching', git will push local branches
  to the remote branches that already exist with the same name.

  In Git 2.0, Git will default to the more conservative 'simple'
  behavior, which only pushes the current branch to the corresponding
  remote branch that 'git pull' uses to update the current branch.

  See 'git help config' and search for 'push.default' for further information.
  (the 'simple' mode was introduced in Git 1.7.11. Use the similar mode
  'current' instead of 'simple' if you sometimes use older versions of Git)

  Username for 'https://github.com': github-account
  Password for 'https://github-account@github.com':
  ```

### Работа с ветками

Когда сделан форк репозитория, у вас по умолчанию создается единственная ветка
`master`. Тем не менее, при решении независимых задач следует создавать
отдельные ветки Git. Далее показаны основные команды для управления ветками на
примере ветки `add-new-google-tests`.

  1. Получить список веток:

  ```bash
  $ git branch [-v]
  # [-v] - список с информацией о последних коммитах
  ```

  1. Создать ветку:

  ```bash
  $ git branch add-new-google-tests
  ```

  1. Перейти в ветку:

  ```bash
  $ git checkout add-new-google-tests
  ```

  1. Создать ветку `add-new-google-tests` и сразу перейти в нее:

  ```bash
  $ git checkout [-b] add-new-google-tests
  # [-b] - создание и переход в ветку <branch_name>
  ```

  1. Удалить ветку в локальном репозитории:

  ```bash
  $ git branch -d <branch_name>
  # [-D] - используется для принудительного удаления веток
  ```

### Работа с изменениями

При работе с файлами в ветке необходимо управлять изменениями. Далее приведен
перечень основных команд, которые могут понадобиться.

  1. Получить список текущих изменений:

  ```bash
  $ git status
  ```

  1. Пометить файл как добавленный в текущую ветку репозитория (файл будет
     добавлен в историю после выполнения команды `git commit`):

  ```bash
  $ git add [<file_name>]
  # <file_name> - название файла для добавления в commit
  #     если вместо имени указан символ *, то будут добавлены все новые файлы,
  #     не совпадающие с масками, указанными в .gitignore
  ```

  1. Добавить изменения в текущую ветку локального репозитория:

  ```bash
  $ git commit [-m "<message_to_commit>"] [-a]
  # [-a] - автоматически добавляет изменения для существующих на сервере файлов
  #     без выполнения команды git add
  # [--amend] - перезаписывает последний коммит (используется, если не забыты
  #     изменения)
  ```

  1. Разместить изменения, которые были добавлены в локальный репозиторий
     с помощью команды `git commit`:

  ```bash
  $ git push [-u] origin [<branch_name>]
  # [-u] - отслеживать версию ветки <branch_name> на удалённом сервере
  #     (origin). Позволяет получать изменения с сервера при помощи команды git pull
  #     без явного указания имени удалённого репозитория и имени ветки.
  ```

  1. Получить изменения с сервера при помощи команды `git pull` и слить их с
     отслеживаемыми ветками:

  ```bash
  $ git pull [remotename [<branch_name>]]
  ```

  1. Удалить файлы или директории (без опции `-f` для файлов, состояния
     которых совпадают с состояниям на сервере!):

  ```bash
  $ git rm [-f] [--cached]
  # [-f] - принудительное удаление (файла с измененным состоянием)
  # [--cached] - удаление файлов на сервере, но не в локальной директории
  ```

  1. Переименовать файлы (также можно сделать в три команды: `mv`, `git rm`,
     `git add`):

  ```bash
  $ git mv <file_from> <file_to>
  ```

### Слияния и конфликты

Когда в проекте работает несколько человек, то вполне естественная ситуация —
необходимость слияния изменений и разрешение конфликтов.

  1. Слияние (вариант 1):

  ```bash
  $ git merge upstream/master # вливание изменений из ветки upstream в master
  $ git merge master # слияние изменений из ветки master в текущую ветку
  ```

  1. Слияние (вариант 2):

  ```bash
  $ git checkout <branch_name> # переход в ветку <branch_name> (при необходимости)
  $ git rebase <base_branch> [<branch_name>] # слияние изменений из ветки <base_branch> в ветку <branch_name>
  $ git checkout <base_branch>
  $ git merge <branch_name>
  ```

  1. Инструмент для разрешения конфликтов:

  ```bash
  $ git mergetool
  ```

## Сборка проекта с помощью CMake и Microsoft Visual Studio

В данном разделе описана типичная последовательность действий, которую
необходимо выполнить для сборки проекта с использованием утилиты CMake и
Microsoft Visual Studio. Далее для определенности выполняется сборка проекта из
репозитория `itseez-ws-2016-practice`.

### Генерация проекта Microsoft Visual Studio

  1. Рядом с директорией `itseez-ws-2016-practice` создайте
     `itseez-ws-2016-practice-build`. В новой директории будут размещены файлы
     решения и проектов, сгенерированные с помощью CMake.

     ```bash
     $ cd ..
     $ mkdir itseez-ws-2016-practice-build
     ```

  1. Перейдите в директорию `itseez-ws-2016-practice-build`:

     ```bash
     $ cd ./itseez-ws-2016-practice-build
     ```

  1. Сгенерируйте файлы решения и проектов с помощью утилиты CMake. Для этого
     можно воспользоваться графическим приложением, входящим в состав
     утилиты, либо выполнить следующую команду:

  ```bash
  $ cmake -DOpenCV_DIR="<OpenCVConfig.cmake-path>" -G <generator-name> <path-to-repo>
  # <OpenCVConfig.cmake-path> - директория, в которой установлена
  #     библиотека OpenCV и расположен файл OpenCVConfig.cmake
  # <generator-name> - название генератора, в случае тестовой
  #     инфраструктуры участников школы может быть "Visual Studio 10 Win64"
  #     (если в командной строке набрать cmake без параметров, то можно просмотреть
  #     список доступных генераторов)
  # <path-to-repo> - путь до директории itseez-ws-2016-practice, где лежат
  #     исходные коды проекта (если предыдущие действия
  #     выполнены корректно, то это директория`../itseez-ws-2016-practice`)
  ```

  Примечание: после запуска утилиты CMake на экране появятся сообщения,
  приведенные ниже.

  ```txt
  -- The C compiler identification is MSVC 16.0.30319.1
  -- The CXX compiler identification is MSVC 16.0.30319.1
  -- Check for working C compiler using: Visual Studio 10 Win64
  -- Check for working C compiler using: Visual Studio 10 Win64 -- works
  -- Detecting C compiler ABI info
  -- Detecting C compiler ABI info - done
  -- Check for working CXX compiler using: Visual Studio 10 Win64
  -- Check for working CXX compiler using: Visual Studio 10 Win64 -- works
  -- Detecting CXX compiler ABI info
  -- Detecting CXX compiler ABI info - done
  -- OpenCV ARCH: x64
  -- OpenCV RUNTIME: vc10
  -- OpenCV STATIC: OFF
  -- Found OpenCV 2.4.11 in c:/OpenCV-2.4.11/opencv/build/x64/vc10/lib
  -- You might need to add c:\OpenCV-2.4.11\opencv\build\x64\vc10\bin to your PATH to be able to run your applications.
  --
  -- General configuration for practice1
  -- ======================================
  --
  --    Configuration:        Release
  --    OpenCV build path:    c:\OpenCV-2.4.11\opencv\build
  --
  -- Configuring done
  -- Generating done
  -- Build files have been written to: C:/Users/ws2016/Documents/GitHub/itseez-ws-2016-practice
  ```

### Построение и запуск

  1. Откройте сгенерированный файл решения `itseez-ws-2016-practice.sln`.

  1. Нажмите правой кнопкой мыши по проекту `ALL_BUILD` и выберите пункт
     `Rebuild` контекстного меню, чтобы собрать решение. В результате все
     бинарные файлы будут размещены в директории
     `itseez-ws-2016-practice-build/bin`.

  1. Для запуска приложения и тестов откройте командную строку (`cmd.exe` в
     `Пуск`) и перейдите в директорию с бинарными файлами, используя команду
     `cd`.

  1. Можно запустить демо-приложение. Возможное сообщение при запуске: `The
     program can't start because opencv_imgproc2411d.dll is missing from your
     computer. Try reinstalling the program to fix this problem.`.
     - Решение 1: скопировать соответствующую библиотеку из
       `C:\openCV-2.4.11\opencv\build\x64\vcX\bin` (`vcX` - версия Visual
       Studio, принимает значения `vc10`, `vc11`, `vc12`) к бинарным файлам
       проекта. Потребуется три таких библиотеки `opencv_core2411d.dll`,
       `opencv_highgui2411d.dll`, `opencv_imgproc2411d.dll`.
     - Решение 2: добавить путь `C:\openCV-2.4.11\opencv\build\x64\vcX\bin` (не
       забудьте заменить `vcX` на правильную версию Visual Studio) в переменную
       окружения `PATH`.

  1. По аналогии следует запустить тесты. В результате прохождения тестов
     появится список тестов и статус подобно тому, что показан ниже.

```txt
[==========] Running 2 tests from 1 test case.
[----------] Global test environment set-up.
[----------] 2 tests from skeleton
[ RUN      ] skeleton.cvtcolor_matches_opencv
[       OK ] skeleton.cvtcolor_matches_opencv (0 ms)
[ RUN      ] skeleton.resize_matches_opencv
[       OK ] skeleton.resize_matches_opencv (0 ms)
[----------] 2 tests from skeleton (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test case ran. (0 ms total)
[  PASSED  ] 2 tests.
```

__Примечание:__ генератор проекта должен совпадать с версией Visual Studio,
которая использовалась при сборке OpenCV. В пакете OpenCV доступны библиотеки,
собранные под VS 2010, 2012, 2013.

## Детальная инструкция по выполнению работы

  1. Сделать форк upstream-репозитория.
     1. Открыть в браузере [upstream][upstream]-репозиторий.
     1. В правом верхнем углу окна GitHub нажать кнопку `Fork`.
     1. Выбрать в качестве организации, куда направить форк, организацию,
        соответствующую вашему личному аккаунту `@github-account`.

  1. Клонировать origin-репозиторий к себе на локальную машину.
     1. Открыть командную строку Git Bash (или Git Shell в зависимости от того,
        какой git-клиент установлен на вашей машине). Для этого необходимо найти
        соответствующий ярлык на рабочем столе или в меню "Пуск".
     1. Воспользоваться инструкциями, описанными в разделе
        [Общие инструкции по работе с Git][git-intro].

  1. Собрать проект и проверить его работоспособность, запустив тесты и
     демо-приложение.
     1. Воспользоваться инструкцией по сборке и запуску, описанной в разделе
        [Сборка проекта с помощью CMake и MS VS][cmake-msvs].
     1. Чтобы проверить работоспособность тестов, достаточно запустить
        `skeleton.exe`, `test_skeleton.exe` и `perf_skeleton.exe`. Если все
        тесты "зеленые", то можно двигаться дальше.

        ```bash
        $ cd itseez-ws-2016-practice-build/bin
        $ ./skeleton.exe --save --image ./testdata/page.png
        $ ./test_skeleton.exe
        $ ./perf_skeleton.exe
        ```

  1. Создать рабочую ветку `add-new-google-tests` и перейти в нее для размещения
     новых тестов (подробные инструкции в разделе
     [Общие инструкции по работе с Git][git-intro]).

  1. Для начала предлагается добавить простой фиктивный тест и сделать pull
     request в upstream-репозиторий, чтобы проверить работоспособность тестов на
     Travis-CI. В дальнейшем нужно будет работать по этому же циклу:
     - Откройте файл `itseez-ws-2016-practice/test/test_skeleton.cpp` и добавьте
       в него следующий тест:

       ```cpp
       TEST(skeleton, 2_plus_2_equals_4)
       {
           EXPECT_EQ(4, 2 + 2);
       }
       ```

     - Постройте проект и запустите сборку с тестами, убедитесь, что ваш тест
       запустился и отработал:

       ```txt
       [==========] Running 3 tests from 1 test case.
       [----------] Global test environment set-up.
       [----------] 3 tests from skeleton
       [ RUN      ] skeleton.2_plus_2_equals_4
       [       OK ] skeleton.2_plus_2_equals_4 (0 ms)
       [ RUN      ] skeleton.cvtcolor_matches_opencv
       [       OK ] skeleton.cvtcolor_matches_opencv (5 ms)
       [ RUN      ] skeleton.resize_matches_opencv
       [       OK ] skeleton.resize_matches_opencv (1 ms)
       [----------] 3 tests from skeleton (6 ms total)

       [----------] Global test environment tear-down
       [==========] 3 tests from 1 test case ran. (6 ms total)
       [  PASSED  ] 3 tests.
       ```

     - Далее сделайте коммит, отправьте его на свой форк:

       ```bash
       $ git add ./test/test_skeleton.cpp
       $ git commit -m "Add dummy test"
       $ git push origin HEAD
       ```

     - Заключительным шагом создайте pull request и проверьте, что он успешно
       прошел тестирование на [Travis-CI][travis].

  1. Далее необходимо в том же файле реализовать тесты, описанные в разделе с
     [постановкой задачи][tasks]. Нужно заметить, что эти тесты не несут
     большого прикладного смысла, однако позволяют усвоить основные концепции
     OpenCV и Google Test, которые пригодятся в дальнейшей работе. Для выполнения работы будут полезны следующие источники: [Документация по Google Test][gtest-doc], [Описание структур данных OpenCV][opencv-doc].

  1. При реализации каждого теста следует пересобирать проект и проверять
     работоспособность тестов. Изменения необходимо постоянно фиксировать,
     выкладывая в рабочую ветку на сервер (разделы [Общие инструкции по работе с
     Git][git-intro], [Сборка проекта с помощью CMake и MS VS][cmake-msvs]).
     Pull request будет автоматически обновляться, и вам полезно следить за
     состоянием тестирования на [Travis-CI][travis].

  1. После того, как все тесты будут реализованы, вы можете выбрать и решить
     одну из [Дополнительных задач][tasks].

<!-- LINKS -->

[git]:          https://git-scm.com/book/en/v2
[github]:       https://github.com
[travis]:       https://travis-ci.org/itseez-academy/itseez-ws-2016-practice
[gtest]:        https://github.com/google/googletest
[upstream]:     https://github.com/itseez-academy/itseez-ws-2016-practice
[origin]:       https://github.com/github-account/itseez-ws-2016-practice
[git-intro]:  https://github.com/itseez-academy/itseez-ws-2016-practice/blob/master/docs/practice1-development-tools.md#Общие-инструкции-по-работе-с-git
[cmake-msvs]: https://github.com/itseez-academy/itseez-ws-2016-practice/blob/master/docs/practice1-development-tools.md#Сборка-проекта-с-помощью-cmake-и-microsoft-visual-studio
[tasks]:        https://github.com/itseez-academy/itseez-ws-2016-practice/blob/master/docs/practice1-development-tools.md#Задачи
[gtest-doc]:    https://github.com/google/googletest/blob/master/googletest/docs/Documentation.md
[gtest-prim]:   https://github.com/google/googletest/blob/master/googletest/docs/Primer.md
[opencv-doc]:   http://docs.opencv.org/2.4.11/modules/core/doc/basic_structures.html
[feedback_day1]:https://docs.google.com/forms/d/172QaSZopk-ecXXWF6qVymah_OBhPumVjOTwW4OzzB-g/viewform