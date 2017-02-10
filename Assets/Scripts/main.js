var Wrapper = require('OP'),
	OP = Wrapper.engine,
	GameState = Wrapper.GameState;

var Helper = {
    Log: function (o, ind) {
        if (!ind) {
            ind = 0;
        }
        for (var k in o) {
            var prev = '';
            for (var i = 0; i < ind; i++) prev += '    ';
            if (typeof o[k] == 'object') {
                console.log(prev + k);
                Helper.Log(o[k], ind + 1);
            } else if (typeof o[k] == 'array') {
                console.log(prev + k);
                Helper.Log(o[k], ind + 1);
            } else {
                console.log(prev + k + ':' + o[k]);
            }
        }
    }
};


var GlobalScene;
var Entities = {};

function SetTransform(entity, node, offset, scale) {
    if (entity.ignoreTransform) return;

    console.log(node.opm);

    if (entity.SetTransform) {

        console.log('       Set Transform');
        entity.SetTransform(
            node.position[0] + offset[0],
            node.position[1] + offset[1],
            node.position[2] + offset[2],
            node.rotation[0],
            node.rotation[1],
            node.rotation[2],
            node.scale[0] * scale[0],
            node.scale[1] * scale[1],
            node.scale[2] * scale[2]
        );

    } else {

        console.log('       Entity.Set Transform');
        entity.Entity.SetTransform(
            node.position[0] + offset[0],
            node.position[1] + offset[1],
            node.position[2] + offset[2],
            node.rotation[0],
            node.rotation[1],
            node.rotation[2],
            node.scale[0] * scale[0],
            node.scale[1] * scale[1],
            node.scale[2] * scale[2]
        );
    }
}

function SetSize(entity, node, scale) {
    entity.Entity.SetSize(
        (node.boundingBox.max[0] - node.boundingBox.min[0]) * node.scale[0] * scale[0],
        (node.boundingBox.max[1] - node.boundingBox.min[1]) * node.scale[1] * scale[1],
        (node.boundingBox.max[2] - node.boundingBox.min[2]) * node.scale[2] * scale[2]
        );
}

function Add(fn, node) {
    console.log(GlobalScene.ptr, node.opm, node.material.texture);

    if (node.type == 'GROUP') {
        return GlobalScene[fn](node.opm, node.children[0].material.texture);
    } else {
        return GlobalScene[fn](node.opm, node.material.texture);
    }
}

function LoadSceneEntity(node, offset, scale) {
    
    var deltaOffset = [
        node.position[0] + offset[0],
        node.position[1] + offset[1],
        node.position[2] + offset[2]];

    var deltaScale = [
        node.scale[0] * scale[0],
        node.scale[1] * scale[1],
        node.scale[2] * scale[2]];

    // Non-OPM Group, just load the sub-meshes
    if (node.type == "GROUP" && (!node.opm || node.opm == '')) {
        for (var i = 0; i < node.children.length; i++) {
            LoadSceneEntity(scene, node.children[i], deltaOffset, deltaScale);
        }

        return;
    } else {
        for (var i = 0; i < node.children.length; i++) {
            if (node.children[i].gameType && node.children[i].gameType != '' && node.children[i].gameType != ' ') {
                LoadSceneEntity(scene, node.children[i], deltaOffset, deltaScale);
            }
        }
    }

    var entity = null;

    console.error('gameType: ' + node.gameType);
    // Load up the node by game type
    switch (node.gameType) {
        case 'Player': entity = Add('AddPlayer', node); break;
        case 'Static': entity = Add('AddStatic', node); break;
        default: entity = Add('AddDrawable', node); break;
    }

    if (entity) {
        console.log('entity was set');
        SetTransform(entity, node, offset, scale);
        if (entity.SetSize) {
            console.log('       Set Size');
            entity.SetSize(
                node.boundingBox.max[0] - node.boundingBox.min[0],
                node.boundingBox.max[1] - node.boundingBox.min[1],
                node.boundingBox.max[2] - node.boundingBox.min[2]);
        }
        if (entity.UpdatePhysics) {
            Helper.Log(entity);
            console.log('       Updating Physics');
            entity.UpdatePhysics();
        }
        Entities[node.name] = entity;
    }

    if (node.scripts) {
        if (node.scripts.Activate && entity.SetActivate) {
            console.log(' !!! Active');
            entity.SetActivate(function (player) {
                eval(node.scripts.Activate);
            });
        }
        if (node.scripts.Deactivate && entity.SetDeactivate) {
            console.log(' !!! Deactive');
            entity.SetDeactivate(function (player) {
                eval(node.scripts.Deactivate);
            });
        }
        if (node.scripts.Trigger) {
            console.log(' !!! Trigger');
            entity.SetTrigger(function (player) {
                eval(node.scripts.Trigger);
            });
        }
    }
}

function GameInit(scene) {
    console.log("Game Init called");

    GlobalScene = scene;

    Helper.Log(GlobalScene);

    var result = load('base.opscene');
    for (var i = 0; i < result.models.length; i++) {
        LoadSceneEntity(result.models[i], [0, 0, 0], [1, 1, 1]);
    }
}