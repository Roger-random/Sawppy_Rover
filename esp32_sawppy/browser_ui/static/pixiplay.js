// Playground for following along https://github.com/kittykatattack/learningPixi

var app;
var cat;

function goPixi () {
  // New application instance
  app = new PIXI.Application({width: 200, height: 200});

  // Fill window
  app.renderer.view.style.position = "absolute";
  app.renderer.view.style.display = "block";
  app.renderer.autoResize = true;
  app.renderer.resize(window.innerWidth, window.innerHeight);

  //Add the canvas that Pixi automatically created for you to the HTML document
  document.body.appendChild(app.view);

  cat = new PIXI.Sprite.from("cat.png");

  cat.position.set(100, 100);

  app.stage.addChild(cat);

}

document.addEventListener('DOMContentLoaded', goPixi, false);
