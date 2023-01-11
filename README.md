<div align="center">
  <h1>OpenString</h1>
    <div>
      <a href="https://github.com/1762757171/OpenString/actions/workflows/windows.yml">
            <img src="https://github.com/1762757171/OpenString/actions/workflows/windows.yml/badge.svg" alt="ci-windows" />
      </a>
      <a>
            <img src="https://img.shields.io/badge/progress-15%25-yellow" alt="progress" />
      </a>
    </div>

  <b>A user-friendly string library for game developing, which is based on utf-8 encoding, providing easy-using operating functions and {fmt}-like formatting methods.</b><br/>
</div>

## License

*OpenString* is distributed under the MIT license.

## Features

- User-friendly APIs
- Encoding dependencies are encapsulated and hidden
- Using [{fmt}](https://fmt.dev/latest/index.html)-like formatting method

## Benchmarks

todo

## Q&A

*Q:* Why both Visual Studio solution file and xmake.lua exist?

*A:* This project provides two ways to build, both Visual Studio and XMake are supported. CMake support may be added later.

## TODO

- Folder directory path library
- A ``name`` class for easy copying and comparing
- Localization support

## Thanks

- [{fmt}](https://github.com/fmtlib/fmt): Provides a lot of inspiration for the formatting method.
- [Folly](https://github.com/facebook/folly): FBString provides references for some implementations.
- [xmake](https://github.com/xmake-io/xmake): A powerful cross-platform build utility.