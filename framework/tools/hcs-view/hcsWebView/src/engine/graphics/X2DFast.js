/*
 * Copyright (c) 2022-2023 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { XMat4 } from './XMat4.js';
import { XShader } from './XShader.js';
import { Scr } from '../XDefine.js';
import { XTexture } from './XTexture.js';
import { gl } from '../GLFrame.js';
import { fAngle, iDistance } from '../XTools.js';
const { NapiLog } = require('./../../hcs/NapiLog');
const COLOR = 0xffffffff;
const DRAWTEXT_SIZE = 14;

export class X2DFast {
  static gi() {
    if (X2DFast.px2f === null) {
      X2DFast.px2f = new X2DFast();
    }
    return X2DFast.px2f;
  }

  constructor() {
    this.localBuffer = gl.createBuffer();
    this.texSampleIdx = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

    this.vertexArray = new ArrayBuffer(1024 * 1024 * 4 * 2);
    this.vertexFloat32 = new Float32Array(this.vertexArray);
    this.vertexUint32 = new Uint32Array(this.vertexArray);
    this.whiteImg = XTexture.gi().loadTextureFromImage('CUSTOM_TEXTURE_1');
    this.whiteCut = XTexture.gi().makeCut(this.whiteImg, 0, 0, 1, 1);
    XShader.gi();

    this.resetMat();
  }
  resetMat() {
    X2DFast.transform2D.unit();
    X2DFast.transform2D.orthoMat(0, 0, Scr.logicw, Scr.logich);
    let tm = X2DFast.transform2D.mat;
    this.t2dExt = [
      tm[0][0],
      tm[1][0],
      tm[2][0],
      tm[3][0],
      tm[0][1],
      tm[1][1],
      tm[2][1],
      tm[3][1],
      tm[0][2],
      tm[1][2],
      tm[2][2],
      tm[3][2],
      tm[0][3],
      tm[1][3],
      tm[2][3],
      tm[3][3],
    ];
  }

  swapMode2D() {
    gl.disable(gl.DEPTH_TEST);

    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
  }

  DrawCircle(ox, oy, rw, rh, c = 0xffffffff, lw = 1) {
    let lx = -1;
    let ly = -1;
    let i = 0;
    let gap = (Math.PI * 2) / 32;
    while (i < Math.PI * 2 + 0.00001) {
      let dx = Math.cos(i) * rw + ox;
      let dy = Math.sin(i) * rh + oy;
      if (lx !== -1) {
        this.drawLine(lx, ly, dx, dy, c, lw);
      }
      lx = dx;
      ly = dy;
      i += gap;
    }
  }

  fillRect(x, y, w, h, c = 0xffffffff) {
    this.drawCut(this.whiteCut, x, y, w, h, 0, 0, 0, c);
  }

  drawLine(x1, y1, x2, y2, c = 0xffffffff, linewidth = 1) {
    this.drawCut(
      this.whiteCut,
      x1,
      y1,
      iDistance(x1 - x2, y1 - y2),
      linewidth,
      fAngle(x2 - x1, y2 - y1),
      0,
      0.5,
      c
    );
  }

  drawRect(x, y, w, h, c = 0xffffffff, lw = 1) {
    this.drawLine(x - lw / 2, y, x + w + lw / 2, y, c, lw);
    this.drawLine(x, y, x, y + h, c, lw);
    this.drawLine(x + w, y + h, x + w, y, c, lw);
    this.drawLine(x + w + lw / 2, y + h, x - lw / 2, y + h, c, lw);
  }

  static testTransform(x, y, sw, sh, ra, ox, oy, realw, realh) {
    const LEFT = -1;
    const MIDDLE = -2;
    const RIGHT = -3;
    const UP = -1;
    const DOWN = -3;
    const BY_MIDDLE = 2;
    X2DFast.tmpMat.unit();
    if (ox === LEFT) {
      ox = 0;
    }
    if (ox === MIDDLE) {
      ox = Math.floor(realw / BY_MIDDLE);
    }
    if (ox === RIGHT) {
      ox = realw;
    }
    if (oy === UP) {
      oy = 0;
    }
    if (oy === MIDDLE) {
      oy = Math.floor(realh / BY_MIDDLE);
    }
    if (oy === DOWN) {
      oy = realh;
    }
    if (ox !== 0 || oy !== 0) {
      X2DFast.tmpMat.move(-ox, -oy, 0);
    }
    if (sw !== 1 || sh !== 1) {
      X2DFast.tmpMat.scale(sw, sh, 1);
    }
    if (ra !== 0) {
      X2DFast.tmpMat.rotate(0, 0, ra);
    }
    if (x !== 0 || y !== 0) {
      X2DFast.tmpMat.move(x, y, 0);
    }
  }
  clearBuffer() {
    this.ridDict = {};
    this.ridPoint = 0;
    this.drawCount = 0;
  }
  swapC(c) {
    let r = Math.floor((((c >> 16) & 0xff) * 63) / 255);
    let g = Math.floor((((c >> 8) & 0xff) * 63) / 255);
    let b = Math.floor(((c & 0xff) * 63) / 255);
    let a = Math.floor((((c >> 24) & 0xff) * 63) / 255);
    return ((a * 64 + r) * 64 + g) * 64 + b;
  }
  drawCut_(pcut, m00, m01, m10, m11, m22, m30, m31, c = COLOR) {
    if (c === -1) {
      c = COLOR;
    }
    c = this.swapC(c);
    this.vertexFloat32.set([0.0, 0.0, 0.0, pcut.u0, pcut.v0, m00, m01, m10, m11, m22, m30, m31,
      this.ridDict[pcut.rid], c,
        pcut.w, 0.0, 0.0, pcut.u1, pcut.v1, m00, m01, m10, m11, m22, m30, m31, this.ridDict[pcut.rid], c,
        pcut.w, pcut.h, 0.0, pcut.u2, pcut.v2, m00, m01, m10, m11, m22, m30, m31, this.ridDict[pcut.rid], c,
        0.0, 0.0, 0.0, pcut.u0, pcut.v0, m00, m01, m10, m11, m22, m30, m31, this.ridDict[pcut.rid], c,
        pcut.w, pcut.h, 0.0, pcut.u2, pcut.v2, m00, m01, m10, m11, m22, m30, m31, this.ridDict[pcut.rid], c,
      0.0, pcut.h, 0.0, pcut.u3, pcut.v3, m00, m01, m10, m11, m22, m30, m31, this.ridDict[pcut.rid], c,],
    this.drawCount * 14 * 6);
    this.drawCount += 1;
  }
  drawCutEx(cid, tmat, c = COLOR) {
    let pcut = XTexture.pinstance_.allCuts[cid];
    if (!(pcut.rid in this.ridDict)) {
      this.ridDict[pcut.rid] = this.ridPoint;
      this.ridPoint += 1;
    }
    tmat = tmat.mat;
    this.drawCut(pcut, tmat[0][0], tmat[0][1], tmat[1][0], tmat[1][1], tmat[2][2], tmat[3][0], tmat[3][1], c);
  }
  drawCut(cid, x = 0, y = 0, sw = 1, sh = 1, ra = 0, ox = 0, oy = 0, c = COLOR) {
    let intX = parseInt(x);
    let intY = parseInt(y);
    let pcut = XTexture.gi().allCuts[cid];
    if (pcut === null) {
      NapiLog.logError('error occured getting object');
      return;
    }
    if (!(pcut.rid in this.ridDict)) {
      if (this.ridPoint >= 16) {
        this.freshBuffer();
        this.clearBuffer();
      }
      this.ridDict[pcut.rid] = this.ridPoint;
      this.ridPoint += 1;
    }
    X2DFast.testTransform(intX, intY, sw, sh, ra, ox, oy, pcut.w, pcut.h);
    let tmat = X2DFast.tmpMat.mat;
    this.drawCut_(pcut, tmat[0][0], tmat[0][1], tmat[1][0], tmat[1][1], tmat[2][2], tmat[3][0], tmat[3][1], c);
  }
  drawText(s, size = DRAWTEXT_SIZE, x = 0, y = 0, sw = 1, sh = 1, ra = 0, ox = 0, oy = 0, c = COLOR) {
    if (s.length <= 0) {
      NapiLog.logError('error occured s is null');
      return 0;
    }
    let cid = XTexture.gi().getText(s, size);
    if (cid >= 0) {
      this.drawCut(cid, x, y, sw, sh, ra, ox, oy, c);
    }
    return XTexture.gi().allCuts[cid].w;
  }
  getTextWidth(s, size) {
    if (s.length <= 0) {
      return 0;
    }
    let cid = XTexture.gi().getText(s, size);
    return XTexture.gi().allCuts[cid].w;
  }
  freshBuffer() {
    XTexture.gi()._FreshText();
    if (this.drawCount === 0) {
      return;
    }
    let ps = XShader.gi().use(XShader.ID_SHADER_FAST);
    for (let rid in this.ridDict) {
      gl.activeTexture(gl.TEXTURE0 + this.ridDict[rid]);
      gl.bindTexture(gl.TEXTURE_2D, XTexture.gi().ximages[rid].tex);

      gl.uniform1i(ps.tex[this.ridDict[rid]], this.ridDict[rid]);
    }

    gl.uniformMatrix4fv(ps.uMat, false, this.t2dExt);

    gl.bindBuffer(gl.ARRAY_BUFFER, this.localBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, this.vertexArray, gl.STATIC_DRAW);
    gl.vertexAttribPointer(ps.position, 3, gl.FLOAT, false, 4 * 14, 0);
    gl.enableVertexAttribArray(ps.position);
    gl.vertexAttribPointer(ps.aTexCoord, 2, gl.FLOAT, false, 4 * 14, 4 * 3);
    gl.enableVertexAttribArray(ps.aTexCoord);
    gl.vertexAttribPointer(ps.ext1, 4, gl.FLOAT, false, 4 * 14, 4 * 5);
    gl.enableVertexAttribArray(ps.ext1);
    gl.vertexAttribPointer(ps.ext2, 4, gl.FLOAT, false, 4 * 14, 4 * 9);
    gl.enableVertexAttribArray(ps.ext2);
    gl.vertexAttribPointer(ps.inColor, 1, gl.FLOAT, false, 4 * 14, 4 * 13);
    gl.enableVertexAttribArray(ps.inColor);

    gl.drawArrays(gl.TRIANGLES, 0, 6 * this.drawCount);
  }
}
X2DFast.tmpMat = new XMat4();
X2DFast.transform2D = new XMat4();

X2DFast.px2f = null;
