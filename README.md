# ChunithmPatch

Chunithm 遊戲的 Discord Rich Presence 補丁，可在 Discord 顯示你正在遊玩的歌曲、難度、分數等資訊。

## 使用方式

1. 下載或編譯 `ChunithmPatch.dll`
2. 將 DLL 注入到遊戲程序中
3. 開始遊玩，Discord 狀態將自動更新

## 專輯封面設定（可選）

若要在 Discord 顯示歌曲封面圖片，請在 DLL 同目錄下建立 `jacket-urls.json` 檔案：

```json
[
  {
    "idx": 123,
    "title": "歌曲名稱",
    "imageUrl": "https://example.com/image.jpg"
  }
]
```

- `idx`：歌曲 ID
- `title`：歌曲名稱
- `imageUrl`：封面圖片網址

## 顯示內容

Discord 狀態將顯示：

- 當前歌曲名稱
- 難度等級（BASIC / ADVANCED / EXPERT / MASTER / ULTIMA / WORLD'S END）
- 即時分數與評級（D ~ SSS+）
- 歌曲封面（如有設定）
- 遊戲時間
