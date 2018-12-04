({
    baseUrl: ".",
    name: "vendor/almond",
    include: "require_main",
    mainConfigFile: "require_main.js",
    out: "../../../webapp/assets/js/require_main.built.js",
    findNestedDependencies: true,
    wrapShim: true,
     optimize: "none"
})
