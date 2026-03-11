workspace "Фитнес-трекер" "Архитектура системы фитнес-трекера" {

    model {

        user = person "Пользователь" "Пользователь приложения для отслеживания тренировок"

        fitnessSystem = softwareSystem "Система фитнес-трекера" "Приложение для хранения тренировок и упражнений" {

            webApp = container "Веб/Мобильный клиент" "Пользовательский интерфейс приложения" "React / Мобильное приложение"

            backend = container "Backend API" "Обрабатывает запросы и реализует бизнес-логику" "Spring Boot / Node.js"

            database = container "База данных" "Хранит пользователей, тренировки и упражнения" "PostgreSQL"

            analytics = container "Сервис аналитики" "Рассчитывает статистику тренировок" "Python"

            webApp -> backend "Отправляет REST-запросы"
            backend -> database "Чтение и запись данных"
            backend -> analytics "Запрашивает статистику"
            analytics -> database "Читает данные для анализа"
        }

        user -> webApp "Использует приложение"
    }

    views {

        systemContext fitnessSystem {
            include *
            autolayout lr
        }

        container fitnessSystem {
            include *
            autolayout lr
        }

        dynamic fitnessSystem {

            user -> webApp "Создаёт тренировку"
            webApp -> backend "POST /workouts"
            backend -> database "Сохраняет тренировку"
            backend -> webApp "Возвращает id тренировки"

            webApp -> backend "Добавляет упражнение"
            backend -> database "Сохраняет упражнение в тренировке"

            autolayout lr
        }

        theme default
    }
}