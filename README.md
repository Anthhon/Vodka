# Vodka

This is a (until now) simple and light-weight framework made in C. I don't recommend using it for professional purposes but if you want to have some fun, move on and try it! The project goal is to be as easy to use as Django with the pure C speed.

# Usage

To use the project just use these commands:

```bash
git clone git@github.com:Anthhon/Vodka.git
cd Vodka
make build-linux -B # Doesn't have a proper Windows build command
cd build

# Change your 'src/config.c' file before running
./Vodka run
```

# Features

- Handle all HTTP methods
    - [X] GET requests
    - [ ] POST requests
    - [ ] PUT requests
    - [ ] DELETE requests
- Handle all file type
    - [X] HTML files
    - [X] CSS files
    - [X] JS files
    - [ ] CSV files
    - [ ] Media files (MP4, PNG, WEBM...)
- Caching
    - [ ] Session management
    - [ ] File reading cache
    - [ ] Http caching
- [X] Allow user to add custom URLs
- [X] Handle 404 errors
- [X] Thread-pool to handle multiple requests
- [X] Logging system to track and record events for debugging
- [ ] Requests rate limiting from single client
- [ ] Dynamic `config.properties` file (confuse.h)
- [ ] Suport for HTTPS (SSL/TLS)
- [ ] Templating system

# Contributing

Contributions to the project would be really appreciated. Talking about your wanted features or changes 
in the [Issues](/issues) tab would be appreciated. Besides that, fork the project and do whatever you want!

# License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.
