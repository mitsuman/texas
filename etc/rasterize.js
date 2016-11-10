var page = require('webpage').create(),
    system = require('system'),
    address, output, size;

if (system.args.length < 3 || system.args.length > 5) {
    console.log('Usage: rasterize.js URL filename [width*height] [zoom]');
    phantom.exit(1);
} else {
    address = system.args[1];
    output = system.args[2];
    //page.viewportSize = { width: 500-18, height: 260 };

    size = system.args[3].split('x');
    page.viewportSize.width = size[0];
    page.viewportSize.height = size[1];

    page.clipRect = {width:size[0], height:size[1]};

    if (system.args.length > 4) {
        page.zoomFactor = system.args[4];
    }
    page.open(address, function (status) {
        if (status !== 'success') {
            console.log('Unable to load the address!');
            phantom.exit();
        } else {
            window.setTimeout(function () {
                page.render(output);
                phantom.exit();
            }, 200);
        }
    });
}
