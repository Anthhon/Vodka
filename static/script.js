let blackTheme = false;

function flipTheme() { blackTheme = !blackTheme; }

function changeColorMode() {
    flipTheme();

    document.body.style.opacity = 0;
    document.body.style.scale = '90%';

    const SECONDARY_COLOR = blackTheme ? '#191919' : '#fff';
    const PRIMARY_COLOR = blackTheme ? '#fff' : '#191919';
    const AUXILIARY_COLOR = blackTheme ? '#ffdd33' : '#3b33ff';

    setTimeout(() => {
        document.body.style.opacity = 100;
        document.body.style.scale = '100%';
    }, 250);

    document.documentElement.style.setProperty('--secondary-color', SECONDARY_COLOR);
    document.documentElement.style.setProperty('--primary-color', PRIMARY_COLOR);
    document.documentElement.style.setProperty('--auxiliary-color', AUXILIARY_COLOR);
}
