/*
 * Copyright (C) 2016 ~ 2018 Wuhan Deepin Technology Co., Ltd.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "musiclistdatawidget.h"

#include <QDebug>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMimeData>
#include <QResizeEvent>
#include <QStandardItemModel>

#include <DLabel>
#include <DPushButton>
#include <DToolButton>
#include <DComboBox>
#include <DLabel>
#include <DFrame>

#include "util/pinyinsearch.h"

#include "widget/playlistview.h"
#include "widget/musiclistdataview.h"
#include "widget/ddropdown.h"
#include "widget/musicimagebutton.h"

#include "util/pinyinsearch.h"

#include "widget/playlistview.h"
#include "widget/musiclistdataview.h"
#include "widget/ddropdown.h"
#include "widget/musicimagebutton.h"

DWIDGET_USE_NAMESPACE

class MusicListDataWidgetPrivate
{
public:
    MusicListDataWidgetPrivate(MusicListDataWidget *parent) : q_ptr(parent) {}

    void initData(PlaylistPtr playlist, bool selectFlag = false, QString searchStr = "");
    int updateInfo();
    void initConntion();
    void showEmptyHits();

    DLabel              *emptyHits      = nullptr;
    DLabel             *emptySearchHits = nullptr;
    DWidget             *actionBar      = nullptr;
    DLabel              *titleLabel     = nullptr;
    DDropdown           *albumDropdown  = nullptr;
    DDropdown           *artistDropdown = nullptr;
    DDropdown           *musicDropdown  = nullptr;
    DDropdown           *albumSearchDropdown  = nullptr;
    DDropdown           *artistSearchDropdown = nullptr;
    DDropdown           *musicSearchDropdown  = nullptr;
    DPushButton         *btPlayAll      = nullptr;
    DLabel              *infoLabel      = nullptr;
    DToolButton    *btIconMode     = nullptr;
    DToolButton    *btlistMode     = nullptr;
    MusicListDataView   *albumListView  = nullptr;
    MusicListDataView   *artistListView = nullptr;
    PlayListView        *musicListView  = nullptr;
    PlayListView        *songListView  = nullptr;
    MusicListDataView   *singerListView  = nullptr;
    MusicListDataView   *albListView  = nullptr;
    DTabWidget          *tabWidget       = nullptr;
    QAction             *customAction   = nullptr;
    PlaylistPtr         curPlaylist     = nullptr;
    PlaylistPtr         selectPlaylist  = nullptr;

    bool                updateFlag         = false;

    MusicListDataWidget *q_ptr;
    Q_DECLARE_PUBLIC(MusicListDataWidget)
};

int MusicListDataWidgetPrivate::updateInfo()
{
    if (updateFlag == false) {

        if (curPlaylist == nullptr)
            return 0;
        PlaylistPtr playlist = curPlaylist;
        QString searchStr = playlist->searchStr();

        bool chineseFlag = false;
        for (auto ch : searchStr) {
            if (DMusic::PinyinSearch::isChinese(ch)) {
                chineseFlag = true;
                break;
            }
        }

        int allCount = 0;
        QFontMetrics titleFm(titleLabel->font());
        auto text = titleFm.elidedText(playlist->displayName(), Qt::ElideRight, 300);
        titleLabel->setText(text);
        titleLabel->setToolTip(playlist->displayName());
        DDropdown *t_curDropdown = nullptr;
        if (playlist->id() == AlbumMusicListID) {
            PlayMusicTypePtrList playMusicTypePtrList = playlist->playMusicTypePtrList();
            int musicCount = 0;
            int musicListCount = 0;
            for (auto action : playMusicTypePtrList) {
                if (searchStr.isEmpty()) {
                    musicCount += action->playlistMeta.sortMetas.size();
                    musicListCount++;
                } else {
                    if (chineseFlag) {
                        if (action->name.contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount += action->playlistMeta.sortMetas.size();
                            musicListCount++;
                        }
                    } else {
                        if (searchStr.size() == 1) {
                            auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                            if (!curTextList.isEmpty() && curTextList.first().contains(searchStr, Qt::CaseInsensitive)) {
                                musicCount += action->playlistMeta.sortMetas.size();
                                musicListCount++;
                            }
                        } else {
                            auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                            if (!curTextList.isEmpty() && curTextList.join("").contains(searchStr, Qt::CaseInsensitive)) {
                                musicCount += action->playlistMeta.sortMetas.size();
                                musicListCount++;
                            }
                        }
                    }
                }
            }
            QString infoStr;
            if (musicCount == 0) {
                infoStr = QString("   ") + MusicListDataWidget::tr("No songs");
            } else if (musicCount == 1) {
                infoStr = QString("   ") + MusicListDataWidget::tr("1 album - 1 song");
            } else {
                infoStr = QString("   ") + MusicListDataWidget::tr("%1 album - %2 songs").arg(musicListCount).arg(musicCount);
                if (musicListCount == 1) {
                    infoStr = QString("   ") + MusicListDataWidget::tr("%1 album - %2 songs").arg(musicListCount).arg(musicCount);
                } else {
                    infoStr = QString("   ") + MusicListDataWidget::tr("%1 albums - %2 songs").arg(musicListCount).arg(musicCount);
                }
            }
            allCount = musicListCount;
            infoLabel->setText(infoStr);
        } else if (playlist->id() == ArtistMusicListID) {
            PlayMusicTypePtrList playMusicTypePtrList = playlist->playMusicTypePtrList();
            int musicCount = 0;
            int musicListCount = 0;
            for (auto action : playMusicTypePtrList) {
                if (searchStr.isEmpty()) {
                    musicCount += action->playlistMeta.sortMetas.size();
                    musicListCount++;
                } else {
                    if (chineseFlag) {
                        if (action->name.contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount += action->playlistMeta.sortMetas.size();
                            musicListCount++;
                        }
                    } else {
                        if (searchStr.size() == 1) {
                            auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                            if (!curTextList.isEmpty() && curTextList.first().contains(searchStr, Qt::CaseInsensitive)) {
                                musicCount += action->playlistMeta.sortMetas.size();
                                musicListCount++;
                            }
                        } else {
                            auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                            if (!curTextList.isEmpty() && curTextList.join("").contains(searchStr, Qt::CaseInsensitive)) {
                                musicCount += action->playlistMeta.sortMetas.size();
                                musicListCount++;
                            }
                        }
                    }
                }
            }
            QString infoStr;
            if (musicCount == 0) {
                infoStr = QString("   ") + MusicListDataWidget::tr("No songs");
            } else if (musicCount == 1) {
                infoStr = QString("   ") + MusicListDataWidget::tr("1 artist - 1 song");
            } else {
                if (musicListCount == 1) {
                    infoStr = QString("   ") + MusicListDataWidget::tr("%1 artist - %2 songs").arg(musicListCount).arg(musicCount);
                } else {
                    infoStr = QString("   ") + MusicListDataWidget::tr("%1 artists - %2 songs").arg(musicListCount).arg(musicCount);
                }
            }
            allCount = musicListCount;
            infoLabel->setText(infoStr);
        } else {
            QString infoStr;
            int musicCount = 0;
            for (auto action : playlist->allmusic()) {
                if (searchStr.isEmpty()) {
                    musicCount ++;
                } else {
                    if (chineseFlag) {
                        if (action->title.contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount++;
                        }
                    } else {
                        if (searchStr.size() == 1) {
                            auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->title);
                            if (!curTextList.isEmpty() && curTextList.first().contains(searchStr, Qt::CaseInsensitive)) {
                                musicCount++;
                            }
                        } else {
                            auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->title);
                            if (!curTextList.isEmpty() && curTextList.join("").contains(searchStr, Qt::CaseInsensitive)) {
                                musicCount++;
                            }
                        }
                    }
                }
            }
            if (musicCount == 0) {
                infoStr = QString("   ") + MusicListDataWidget::tr("No songs");
            } else if (musicCount == 1) {
                infoStr = QString("   ") + MusicListDataWidget::tr("1 song");
            } else {
                infoStr = QString("   ") + MusicListDataWidget::tr("%1 songs").arg(musicCount);
            }
            allCount = musicCount;
            infoLabel->setText(infoStr);
        }

        return allCount;
    }
    return 0;
}

void MusicListDataWidgetPrivate::initData(PlaylistPtr playlist, bool selectFlag, QString searchStr)
{
    Q_Q(MusicListDataWidget);

    if (searchStr == "noSearchResults") {
        actionBar->hide();
        albumListView->hide();
        artistListView->hide();
        musicListView->hide();
        tabWidget->hide();
        emptyHits->show();
        return;
    } else {
        emptyHits->hide();
        actionBar->show();
    }

    PlaylistPtr updatePlaylist  = nullptr;

    if (playlist == nullptr) {
        return;
    }
    curPlaylist = playlist;

    if (updateFlag == true) {
        updatePlaylist = playlist;
    }

    auto preSearchStr = playlist->searchStr();
    playlist->setSearchStr(searchStr);

    int allCount = updateInfo();

    DDropdown *t_curDropdown = nullptr;
    if (playlist->id() == AlbumMusicListID) {

        updateFlag = false;

        artistListView->clearSelection();
        musicListView->clearSelection();

        selectPlaylist  = playlist;

        albumDropdown->show();
        artistDropdown->hide();
        musicDropdown->hide();

        albumSearchDropdown->hide();
        artistSearchDropdown->hide();
        musicSearchDropdown->hide();

        albumListView->show();
        artistListView->hide();
        musicListView->hide();
        tabWidget->hide();

        t_curDropdown = albumDropdown;

        if (albumListView->viewMode() == QListView::IconMode) {
            btIconMode->setChecked(true);
            btlistMode->setChecked(false);
        } else {
            btIconMode->setChecked(false);
            btlistMode->setChecked(true);
        }

        if (!selectFlag || albumListView->listSize() != allCount || preSearchStr != searchStr)
            albumListView->onMusiclistChanged(playlist);
    } else if (playlist->id() == ArtistMusicListID) {

        updateFlag = false;

        albumListView->clearSelection();
        musicListView->clearSelection();

        selectPlaylist  = playlist;
        albumDropdown->hide();
        artistDropdown->show();
        musicDropdown->hide();

        albumSearchDropdown->hide();
        artistSearchDropdown->hide();
        musicSearchDropdown->hide();

        albumListView->hide();
        artistListView->show();
        musicListView->hide();
        tabWidget->hide();

        t_curDropdown = artistDropdown;

        if (artistListView->viewMode() == QListView::IconMode) {
            btIconMode->setChecked(true);
            btlistMode->setChecked(false);
        } else {
            btIconMode->setChecked(false);
            btlistMode->setChecked(true);
        }
        if (!selectFlag || artistListView->listSize() != allCount || preSearchStr != searchStr) {
            artistListView->onMusiclistChanged(playlist);
        }
    } else if (playlist->id() == MusicResultListID || playlist->id() == ArtistResultListID
               || playlist->id() == AlbumResultListID)  {

        albumDropdown->hide();
        artistDropdown->hide();
        musicDropdown->hide();
        musicSearchDropdown->show();

        albumListView->hide();
        artistListView->hide();
        musicListView->hide();
        tabWidget->show();

        if (updateFlag == true) {
            if ( playlist->id() == ArtistResultListID || playlist->id() == AlbumResultListID) {
                musicSearchDropdown->hide();
            }

            if (updatePlaylist->id() == MusicResultListID) {

                if (songListView->viewMode() == QListView::IconMode) {
                    btIconMode->setChecked(true);
                    btlistMode->setChecked(false);
                } else {
                    btIconMode->setChecked(false);
                    btlistMode->setChecked(true);
                }
                songListView->onMusiclistChanged(updatePlaylist);

            } else if (updatePlaylist->id() == ArtistResultListID) {

                if (singerListView->viewMode() == QListView::IconMode) {
                    btIconMode->setChecked(true);
                    btlistMode->setChecked(false);
                } else {
                    btIconMode->setChecked(false);
                    btlistMode->setChecked(true);
                }

                singerListView->onMusiclistChanged(updatePlaylist);

            } else if (updatePlaylist->id() == AlbumResultListID) {

                if (albListView->viewMode() == QListView::IconMode) {
                    btIconMode->setChecked(true);
                    btlistMode->setChecked(false);
                } else {
                    btIconMode->setChecked(false);
                    btlistMode->setChecked(true);
                }

                albListView->onMusiclistChanged(updatePlaylist);
            }
        }

        titleLabel->setText(MusicListDataWidget::tr("Search Results"));

    } else {

        updateFlag = false;

        albumListView->clearSelection();
        artistListView->clearSelection();

        selectPlaylist  = playlist;
        albumDropdown->hide();
        artistDropdown->hide();
        musicDropdown->show();

        albumSearchDropdown->hide();
        artistSearchDropdown->hide();
        musicSearchDropdown->hide();

        albumListView->hide();
        artistListView->hide();
        musicListView->show();
        tabWidget->hide();

        t_curDropdown = musicDropdown;

        if (musicListView->viewMode() != (playlist->viewMode())) {
            musicListView->setViewModeFlag((QListView::ViewMode)playlist->viewMode());
        }

        if (musicListView->viewMode() == QListView::IconMode) {
            btIconMode->setChecked(true);
            btlistMode->setChecked(false);
        } else {
            btIconMode->setChecked(false);
            btlistMode->setChecked(true);
        }
        musicListView->onMusiclistChanged(playlist);
    }
}

void MusicListDataWidgetPrivate::initConntion()
{
    Q_Q(MusicListDataWidget);

    q->connect(albumDropdown, &DDropdown::triggered,
    q, [ = ](QAction * action) {
        albumDropdown->setCurrentAction(action);
        int t_sortType = action->data().toInt() == 0 ? 1 : 0;
        albumListView->playlist()->changePlayMusicTypeOrderType();
        albumListView->playlist()->sortPlayMusicTypePtrListData(t_sortType);
        Q_EMIT q->resort(albumListView->playlist(), action->data().value<Playlist::SortType>());
    });
    q->connect(artistDropdown, &DDropdown::triggered,
    q, [ = ](QAction * action) {
        artistDropdown->setCurrentAction(action);
        int t_sortType = action->data().toInt() == 0 ? 1 : 0;
        artistListView->playlist()->changePlayMusicTypeOrderType();
        artistListView->playlist()->sortPlayMusicTypePtrListData(t_sortType);
        Q_EMIT q->resort(artistListView->playlist(), action->data().value<Playlist::SortType>());
    });
    q->connect(musicDropdown, &DDropdown::triggered,
    q, [ = ](QAction * action) {
        musicDropdown->setCurrentAction(action);
        Q_EMIT q->resort(musicListView->playlist(), action->data().value<Playlist::SortType>());
    });

    /*----------------albumSearchDropdown----------------*/
    q->connect(albumSearchDropdown, &DDropdown::triggered,
    q, [ = ](QAction * action) {
        albumSearchDropdown->setCurrentAction(action);
        int t_sortType = action->data().toInt() == 0 ? 1 : 0;
        albListView->playlist()->changePlayMusicTypeOrderType();
        albListView->playlist()->sortPlayMusicTypePtrListData(t_sortType);
        Q_EMIT q->resort(albListView->playlist(), action->data().value<Playlist::SortType>());
    });
    q->connect(artistSearchDropdown, &DDropdown::triggered,
    q, [ = ](QAction * action) {
        artistSearchDropdown->setCurrentAction(action);
        int t_sortType = action->data().toInt() == 0 ? 1 : 0;
        singerListView->playlist()->changePlayMusicTypeOrderType();
        singerListView->playlist()->sortPlayMusicTypePtrListData(t_sortType);
        Q_EMIT q->resort(singerListView->playlist(), action->data().value<Playlist::SortType>());
    });
    q->connect(musicSearchDropdown, &DDropdown::triggered,
    q, [ = ](QAction * action) {
        musicSearchDropdown->setCurrentAction(action);
        Q_EMIT q->resort(songListView->playlist(), action->data().value<Playlist::SortType>());
    });

    /*-------------btPlayAll--------------------*/
    q->connect(btPlayAll, &DPushButton::clicked,
    q, [ = ](bool) {
        if (albumListView->isVisible()) {
            PlaylistPtr curPlayList = albumListView->playlist();
            if (curPlayList) {
                curPlayList->playMusicTypeToMeta();
                curPlayList->play(curPlayList->first());
                Q_EMIT q->playall(curPlayList);
            }
        } else if (artistListView->isVisible()) {
            PlaylistPtr curPlayList = artistListView->playlist();
            if (curPlayList) {
                curPlayList->playMusicTypeToMeta();
                curPlayList->play(curPlayList->first());
                Q_EMIT q->playall(curPlayList);
            }
        } else if (songListView->isVisible()) {
            if (songListView->playlist()) {
                PlaylistPtr curPlayList = songListView->playlist();
                curPlayList->play(curPlayList->first());
                Q_EMIT q->playall(curPlayList);
            }
        } else if (singerListView->isVisible()) {
            if (singerListView->playlist()) {
                PlaylistPtr curPlayList = singerListView->playlist();
                curPlayList->playMusicTypeToMeta();
                curPlayList->play(curPlayList->first());
                Q_EMIT q->playall(curPlayList);
            }
        } else if (albListView->isVisible()) {
            if (albListView->playlist()) {
                PlaylistPtr curPlayList = albListView->playlist();
                curPlayList->playMusicTypeToMeta();
                curPlayList->play(curPlayList->first());
                Q_EMIT q->playall(curPlayList);
            }
        } else {
            if (musicListView->playlist()) {
                PlaylistPtr curPlayList = musicListView->playlist();
                curPlayList->play(curPlayList->first());
                Q_EMIT q->playall(curPlayList);
            }
        }
    });

    /*-----------------albumListView-----------------------*/

    q->connect(albumListView, &MusicListDataView::requestCustomContextMenu,
    q, [ = ](const QPoint & pos) {
        Q_EMIT q->requestCustomContextMenu(pos, 2);
    });

    q->connect(albumListView, &MusicListDataView::playMedia,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = albumListView->playlist();
        curPlayList->play(meta);
        Q_EMIT q->playMedia(albumListView->playlist(), meta);
    });
    q->connect(albumListView, &MusicListDataView::resume,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = albumListView->playlist();
        Q_EMIT q->resume(albumListView->playlist(), meta);
    });

    q->connect(albumListView, &MusicListDataView::pause,
    q, [ = ](PlaylistPtr playlist, const MetaPtr meta) {
        Q_EMIT q->pause(playlist, meta);
    });

    q->connect(albumListView, &MusicListDataView::addToPlaylist,
    q, [ = ](PlaylistPtr playlist, const MetaPtrList  & metalist) {
        Q_EMIT q->addToPlaylist(playlist, metalist);
    });

    q->connect(albumListView, &MusicListDataView::musiclistRemove,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistRemove(albumListView->playlist(), metalist);
    });

    q->connect(albumListView, &MusicListDataView::musiclistDelete,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistDelete(albumListView->playlist(), metalist);
    });

    q->connect(albumListView, &MusicListDataView::modeChanged,
    q, [ = ](int mode) {
        Q_EMIT q->modeChanged(mode);
    });

    q->connect(albumListView, &MusicListDataView::addMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->addMetasFavourite(metalist);
    });
    q->connect(albumListView, &MusicListDataView::removeMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->removeMetasFavourite(metalist);
    });

    /*-----------------albListView-----------------------*/

    q->connect(albListView, &MusicListDataView::requestCustomContextMenu,
    q, [ = ](const QPoint & pos) {
        Q_EMIT q->requestCustomContextMenu(pos, 5);
    });

    q->connect(albListView, &MusicListDataView::playMedia,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = albListView->playlist();
        curPlayList->play(meta);
        Q_EMIT q->playMedia(albListView->playlist(), meta);
    });
    q->connect(albListView, &MusicListDataView::resume,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = albListView->playlist();
        Q_EMIT q->resume(albListView->playlist(), meta);
    });

    q->connect(albListView, &MusicListDataView::pause,
    q, [ = ](PlaylistPtr playlist, const MetaPtr meta) {
        Q_EMIT q->pause(playlist, meta);
    });

    q->connect(albListView, &MusicListDataView::addToPlaylist,
    q, [ = ](PlaylistPtr playlist, const MetaPtrList  & metalist) {
        Q_EMIT q->addToPlaylist(playlist, metalist);
    });

    q->connect(albListView, &MusicListDataView::musiclistRemove,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistRemove(albListView->playlist(), metalist);
    });

    q->connect(albListView, &MusicListDataView::musiclistDelete,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistDelete(albListView->playlist(), metalist);
    });

    q->connect(albListView, &MusicListDataView::modeChanged,
    q, [ = ](int mode) {
        Q_EMIT q->modeChanged(mode);
    });

    q->connect(albListView, &MusicListDataView::addMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->addMetasFavourite(metalist);
    });
    q->connect(albListView, &MusicListDataView::removeMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->removeMetasFavourite(metalist);
    });

    /*------------------artistListView---------------------------*/

    q->connect(artistListView, &MusicListDataView::requestCustomContextMenu,
    q, [ = ](const QPoint & pos) {
        Q_EMIT q->requestCustomContextMenu(pos, 3);
    });

    q->connect(artistListView, &MusicListDataView::playMedia,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = artistListView->playlist();
        curPlayList->play(meta);
        Q_EMIT q->playMedia(artistListView->playlist(), meta);
    });

    q->connect(artistListView, &MusicListDataView::resume,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = artistListView->playlist();
        Q_EMIT q->resume(artistListView->playlist(), meta);
    });

    q->connect(artistListView, &MusicListDataView::pause,
    q, [ = ](PlaylistPtr playlist, const MetaPtr meta) {
        Q_EMIT q->pause(playlist, meta);
    });

    q->connect(artistListView, &MusicListDataView::addToPlaylist,
    q, [ = ](PlaylistPtr playlist, const MetaPtrList  & metalist) {
        Q_EMIT q->addToPlaylist(playlist, metalist);
    });

    q->connect(artistListView, &MusicListDataView::musiclistRemove,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistRemove(artistListView->playlist(), metalist);
    });

    q->connect(artistListView, &MusicListDataView::musiclistDelete,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistDelete(artistListView->playlist(), metalist);
    });

    q->connect(artistListView, &MusicListDataView::modeChanged,
    q, [ = ](int mode) {
        Q_EMIT q->modeChanged(mode);
    });

    q->connect(artistListView, &MusicListDataView::addMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->addMetasFavourite(metalist);
    });
    q->connect(artistListView, &MusicListDataView::removeMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->removeMetasFavourite(metalist);
    });

    /*------------------singerListView---------------------------*/

    q->connect(singerListView, &MusicListDataView::requestCustomContextMenu,
    q, [ = ](const QPoint & pos) {
        Q_EMIT q->requestCustomContextMenu(pos, 6);
    });

    q->connect(singerListView, &MusicListDataView::playMedia,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = singerListView->playlist();
        curPlayList->play(meta);
        Q_EMIT q->playMedia(singerListView->playlist(), meta);
    });

    q->connect(singerListView, &MusicListDataView::resume,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = singerListView->playlist();
        Q_EMIT q->resume(singerListView->playlist(), meta);
    });

    q->connect(singerListView, &MusicListDataView::pause,
    q, [ = ](PlaylistPtr playlist, const MetaPtr meta) {
        Q_EMIT q->pause(playlist, meta);
    });

    q->connect(singerListView, &MusicListDataView::addToPlaylist,
    q, [ = ](PlaylistPtr playlist, const MetaPtrList  & metalist) {
        Q_EMIT q->addToPlaylist(playlist, metalist);
    });

    q->connect(singerListView, &MusicListDataView::musiclistRemove,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistRemove(singerListView->playlist(), metalist);
    });

    q->connect(singerListView, &MusicListDataView::musiclistDelete,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistDelete(singerListView->playlist(), metalist);
    });

    q->connect(singerListView, &MusicListDataView::modeChanged,
    q, [ = ](int mode) {
        Q_EMIT q->modeChanged(mode);
    });

    q->connect(singerListView, &MusicListDataView::addMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->addMetasFavourite(metalist);
    });
    q->connect(singerListView, &MusicListDataView::removeMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->removeMetasFavourite(metalist);
    });

    /*-----------sing tabBarClicked--------------*/
    q->connect(tabWidget, &DTabWidget::tabBarClicked,
               q, &MusicListDataWidget::resultTabwidget);

    q->connect(musicListView, &PlayListView::pause,
    q, [ = ](const MetaPtr meta) {
        Q_EMIT q->pause(musicListView->playlist(), meta);
    });

    q->connect(songListView, &PlayListView::pause,
    q, [ = ](const MetaPtr meta) {
        Q_EMIT q->pause(songListView->playlist(), meta);
    });

    /*-----------musicListView right click menu------*/

    q->connect(musicListView, &PlayListView::playMedia,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = musicListView->playlist();
        curPlayList->play(meta);
        Q_EMIT q->playMedia(musicListView->playlist(), meta);
    });

    q->connect(musicListView, &PlayListView::resume,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = musicListView->playlist();
        Q_EMIT q->resume(musicListView->playlist(), meta);
    });

    q->connect(musicListView, &PlayListView::pause,
    q, [ = ](const MetaPtr meta) {
        Q_EMIT q->pause(musicListView->playlist(), meta);
    });

    q->connect(musicListView, &PlayListView::requestCustomContextMenu,
    q, [ = ](const QPoint & pos) {
        Q_EMIT q->requestCustomContextMenu(pos, 1);
    });
    q->connect(musicListView, &PlayListView::addToPlaylist,
    q, [ = ](PlaylistPtr playlist, const MetaPtrList  metalist) {
        Q_EMIT q->addToPlaylist(playlist, metalist);
    });
    q->connect(musicListView, &PlayListView::removeMusicList,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistRemove(musicListView->playlist(), metalist);
    });
    q->connect(musicListView, &PlayListView::deleteMusicList,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistDelete(musicListView->playlist(), metalist);
    });
    q->connect(musicListView, &PlayListView::showInfoDialog,
    q, [ = ](const MetaPtr meta) {
        Q_EMIT q->showInfoDialog(meta);
    });
    q->connect(musicListView, &PlayListView::updateMetaCodec,
    q, [ = ](const QString & preTitle, const QString & preArtist, const QString & preAlbum, const MetaPtr  meta) {
        Q_EMIT q->updateMetaCodec(preTitle, preArtist, preAlbum, meta);
    });
    q->connect(musicListView, &PlayListView::addMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->addMetasFavourite(metalist);
    });
    q->connect(musicListView, &PlayListView::removeMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->removeMetasFavourite(metalist);
    });

    q->connect(btIconMode, &DToolButton::clicked,
    q, [ = ](bool) {
        if (albumListView->isVisible()) {
            albumListView->setViewModeFlag(QListView::IconMode);
        } else if (artistListView->isVisible()) {
            artistListView->setViewModeFlag(QListView::IconMode);
        }  else  if (albListView->isVisible()) {
            albListView->setViewModeFlag(QListView::IconMode);
        } else if (singerListView->isVisible()) {
            singerListView->setViewModeFlag(QListView::IconMode);
        } else if (songListView->isVisible())  {
            songListView->setViewModeFlag(QListView::IconMode);
        } else {
            musicListView->playlist()->setViewMode(1);
            musicListView->setViewModeFlag(QListView::IconMode);
        }

        btIconMode->setChecked(true);
        btlistMode->setChecked(false);

    });
    q->connect(btlistMode, &DToolButton::clicked,
    q, [ = ](bool) {
        if (albumListView->isVisible()) {
            albumListView->setViewModeFlag(QListView::ListMode);
        } else if (artistListView->isVisible()) {
            artistListView->setViewModeFlag(QListView::ListMode);
        } else {
            musicListView->playlist()->setViewMode(0);
            musicListView->setViewModeFlag(QListView::ListMode);
        }
        if (albListView->isVisible()) {
            albListView->setViewModeFlag(QListView::ListMode);
        } else if (singerListView->isVisible()) {
            singerListView->setViewModeFlag(QListView::ListMode);
        } else if (songListView->isVisible()) {
            songListView->setViewModeFlag(QListView::ListMode);
        }

        btIconMode->setChecked(false);
        btlistMode->setChecked(true);
    });

    /*-----------songListView right click menu------*/

    q->connect(songListView, &PlayListView::playMedia,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = songListView->playlist();
        curPlayList->play(meta);
        Q_EMIT q->playMedia(songListView->playlist(), meta);
    });

    q->connect(songListView, &PlayListView::resume,
    q, [ = ](const MetaPtr meta) {
        PlaylistPtr curPlayList = songListView->playlist();
        Q_EMIT q->resume(songListView->playlist(), meta);
    });

    q->connect(songListView, &PlayListView::pause,
    q, [ = ](const MetaPtr meta) {
        Q_EMIT q->pause(songListView->playlist(), meta);
    });

    q->connect(songListView, &PlayListView::requestCustomContextMenu,
    q, [ = ](const QPoint & pos) {
        Q_EMIT q->requestCustomContextMenu(pos, 4);
    });
    q->connect(songListView, &PlayListView::addToPlaylist,
    q, [ = ](PlaylistPtr playlist, const MetaPtrList  metalist) {
        Q_EMIT q->addToPlaylist(playlist, metalist);
    });
    q->connect(songListView, &PlayListView::removeMusicList,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistRemove(songListView->playlist(), metalist);
    });
    q->connect(songListView, &PlayListView::deleteMusicList,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->musiclistDelete(songListView->playlist(), metalist);
    });
    q->connect(songListView, &PlayListView::showInfoDialog,
    q, [ = ](const MetaPtr meta) {
        Q_EMIT q->showInfoDialog(meta);
    });
    q->connect(songListView, &PlayListView::updateMetaCodec,
    q, [ = ](const QString & preTitle, const QString & preArtist, const QString & preAlbum, const MetaPtr  meta) {
        Q_EMIT q->updateMetaCodec(preTitle, preArtist, preAlbum, meta);
    });
    q->connect(songListView, &PlayListView::addMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->addMetasFavourite(metalist);
    });
    q->connect(songListView, &PlayListView::removeMetasFavourite,
    q, [ = ](const MetaPtrList  & metalist) {
        Q_EMIT q->removeMetasFavourite(metalist);
    });

    q->connect(btIconMode, &DToolButton::clicked,
    q, [ = ](bool) {
        if (albumListView->isVisible()) {
            albumListView->setViewModeFlag(QListView::IconMode);
        } else if (artistListView->isVisible()) {
            artistListView->setViewModeFlag(QListView::IconMode);
        } else {
            if (songListView->playlist() == nullptr) {
                return;
            }
            songListView->playlist()->setViewMode(1);
            songListView->setViewModeFlag(QListView::IconMode);
        }
        btIconMode->setChecked(true);
        btlistMode->setChecked(false);
    });
    q->connect(btlistMode, &DToolButton::clicked,
    q, [ = ](bool) {
        if (albumListView->isVisible()) {
            albumListView->setViewModeFlag(QListView::ListMode);
        } else if (artistListView->isVisible()) {
            artistListView->setViewModeFlag(QListView::ListMode);
        } else {
            if (songListView->playlist() == nullptr) {
                return;
            }
            songListView->playlist()->setViewMode(0);
            songListView->setViewModeFlag(QListView::ListMode);
        }
        btIconMode->setChecked(false);
        btlistMode->setChecked(true);
    });
}

void MusicListDataWidgetPrivate::showEmptyHits()
{
    if (curPlaylist.isNull() || curPlaylist->searchStr().isEmpty()) {
        actionBar->setVisible(true);
        emptyHits->setVisible(false);
        emptySearchHits->setVisible(false);
        return;
    }
    if (!curPlaylist.isNull() && curPlaylist->id() == AlbumMusicListID) {
        auto t_rowCount = albumListView->rowCount();
        auto playlist = albumListView->playlist();
        if (playlist->searchStr().isEmpty()) {
            emptyHits->setText(MusicListDataWidget::tr("No songs"));
        } else {
            emptyHits->setText(MusicListDataWidget::tr("No search results"));
        }
        bool empty = t_rowCount == 0 ? true : false;
        actionBar->setVisible(!empty);
        albumListView->setVisible(!empty);
        artistListView->setVisible(false);
        musicListView->setVisible(false);
        emptyHits->setVisible(empty);
        emptySearchHits->setVisible(empty && !playlist->searchStr().isEmpty());
    } else if (!curPlaylist.isNull() && curPlaylist->id() == ArtistMusicListID) {
        auto t_rowCount = artistListView->rowCount();
        auto playlist = artistListView->playlist();
        if (playlist->searchStr().isEmpty()) {
            emptyHits->setText(MusicListDataWidget::tr("No songs"));
        } else {
            emptyHits->setText(MusicListDataWidget::tr("No search results"));
        }
        bool empty = t_rowCount == 0 ? true : false;
        actionBar->setVisible(!empty);
        artistListView->setVisible(!empty);
        albumListView->setVisible(false);
        musicListView->setVisible(false);
        emptyHits->setVisible(empty);
        emptySearchHits->setVisible(empty && !playlist->searchStr().isEmpty());
    } else {
        auto t_rowCount = musicListView->rowCount();
        auto playlist = musicListView->playlist();
        if (playlist->searchStr().isEmpty()) {
            emptyHits->setText(MusicListDataWidget::tr("No songs"));
        } else {
            emptyHits->setText(MusicListDataWidget::tr("No search results"));
        }
        bool empty = t_rowCount == 0 ? true : false;
        actionBar->setVisible(!empty);
        artistListView->setVisible(false);
        musicListView->setVisible(!empty);
        emptyHits->setVisible(empty);
    }
}

MusicListDataWidget::MusicListDataWidget(QWidget *parent) :
    DWidget(parent), d_ptr(new MusicListDataWidgetPrivate(this))
{
    Q_D(MusicListDataWidget);

    setObjectName("MusicListDataWidget");
    setAcceptDrops(true);

    setAutoFillBackground(true);
    auto palette = this->palette();
    QColor background("#FFFFFF");
    background.setAlphaF(0.1);
    palette.setColor(DPalette::Background, background);
    setPalette(palette);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 8, 0);
    layout->setSpacing(0);

    d->actionBar = new DWidget;
    d->actionBar->setFixedHeight(80);
    d->actionBar->setObjectName("MusicListDataActionBar");

    auto actionBarLayout = new QVBoxLayout(d->actionBar);
    actionBarLayout->setContentsMargins(10, 3, 8, 0);
    actionBarLayout->setSpacing(0);

    auto actionTileBarLayout = new QHBoxLayout();
    actionTileBarLayout->setContentsMargins(10, 3, 8, 0);
    actionTileBarLayout->setSpacing(0);

    d->titleLabel = new DLabel;
    auto titleFont = d->titleLabel->font();
    titleFont.setFamily("SourceHanSansSC");
    titleFont.setWeight(QFont::Medium);
    titleFont.setPixelSize(24);
    d->titleLabel->setFont(titleFont);
    d->titleLabel->setFixedHeight(36);
    d->titleLabel->setFixedWidth(300);
    d->titleLabel->setObjectName("MusicListDataTitle");
    d->titleLabel->setText(tr("All Music"));
    d->titleLabel->setForegroundRole(DPalette::BrightText);

    d->albumDropdown = new DDropdown;
    d->albumDropdown->setFixedHeight(28);
    d->albumDropdown->setMinimumWidth(130);
    d->albumDropdown->setObjectName("MusicListAlbumDataSort");
    d->albumDropdown->addAction(tr("Time added"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAddTime));
    d->albumDropdown->addAction(tr("Album"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByTitle));
    d->albumDropdown->setCurrentAction();
    d->albumDropdown->hide();

    d->artistDropdown = new DDropdown;
    d->artistDropdown->setFixedHeight(28);
    d->artistDropdown->setMinimumWidth(130);
    d->artistDropdown->setObjectName("MusicListArtistDataSort");
    d->artistDropdown->addAction(tr("Time added"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAddTime));
    d->artistDropdown->addAction(tr("Artist"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByArtist));
    d->artistDropdown->setCurrentAction();
    d->artistDropdown->hide();

    d->musicDropdown = new DDropdown;
    d->musicDropdown->setFixedHeight(28);
    d->musicDropdown->setMinimumWidth(130);
    d->musicDropdown->setObjectName("MusicListMusicDataSort");
    d->musicDropdown->addAction(tr("Time added"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAddTime));
    d->musicDropdown->addAction(tr("Title"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByTitle));
    d->musicDropdown->addAction(tr("Artist"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByArtist));
    d->musicDropdown->addAction(tr("Album"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAblum));
    d->musicDropdown->setCurrentAction();
    /*-------New sorting function-----------*/
    d->albumSearchDropdown = new DDropdown;
    d->albumSearchDropdown->setFixedHeight(28);
    d->albumSearchDropdown->setMinimumWidth(130);
    d->albumSearchDropdown->setObjectName("MusicListAlbumDataSort");
    d->albumSearchDropdown->addAction(tr("Time added"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAddTime));
    d->albumSearchDropdown->addAction(tr("Album"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByTitle));
    d->albumSearchDropdown->setCurrentAction();
    d->albumSearchDropdown->hide();

    d->artistSearchDropdown = new DDropdown;
    d->artistSearchDropdown->setFixedHeight(28);
    d->artistSearchDropdown->setMinimumWidth(130);
    d->artistSearchDropdown->setObjectName("MusicListArtistDataSort");
    d->artistSearchDropdown->addAction(tr("Time added"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAddTime));
    d->artistSearchDropdown->addAction(tr("Artist"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByArtist));
    d->artistSearchDropdown->setCurrentAction();
    d->artistSearchDropdown->hide();

    d->musicSearchDropdown = new DDropdown;
    d->musicSearchDropdown->setFixedHeight(28);
    d->musicSearchDropdown->setMinimumWidth(130);
    d->musicSearchDropdown->setObjectName("MusicListMusicDataSort");
    d->musicSearchDropdown->addAction(tr("Time added"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAddTime));
    d->musicSearchDropdown->addAction(tr("Title"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByTitle));
    d->musicSearchDropdown->addAction(tr("Artist"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByArtist));
    d->musicSearchDropdown->addAction(tr("Album"), QVariant::fromValue<Playlist::SortType>(Playlist::SortByAblum));
    d->musicSearchDropdown->setCurrentAction();
    d->musicSearchDropdown->hide();

    actionTileBarLayout->addWidget(d->titleLabel, 0, Qt::AlignLeft | Qt::AlignBottom);
    actionTileBarLayout->addStretch();

    actionTileBarLayout->addWidget(d->albumDropdown, 0, Qt::AlignRight | Qt::AlignBottom);
    actionTileBarLayout->addWidget(d->artistDropdown, 0, Qt::AlignRight | Qt::AlignBottom);
    actionTileBarLayout->addWidget(d->musicDropdown, 0, Qt::AlignRight | Qt::AlignBottom);

    actionTileBarLayout->addWidget(d->albumSearchDropdown, 0, Qt::AlignRight | Qt::AlignBottom);
    actionTileBarLayout->addWidget(d->artistSearchDropdown, 0, Qt::AlignRight | Qt::AlignBottom);
    actionTileBarLayout->addWidget(d->musicSearchDropdown, 0, Qt::AlignRight | Qt::AlignBottom);

    auto actionInfoBarLayout = new QHBoxLayout();
    actionInfoBarLayout->setContentsMargins(10, 3, 8, 0);
    actionInfoBarLayout->setSpacing(0);

    d->btPlayAll = new DPushButton;
    auto playAllPalette = d->btPlayAll->palette();
    playAllPalette.setColor(DPalette::ButtonText, Qt::white);
    playAllPalette.setColor(DPalette::Dark, QColor("#FD5E5E"));
    playAllPalette.setColor(DPalette::Light, QColor("#ED5656"));
    d->btPlayAll->setPalette(playAllPalette);
    d->btPlayAll->setIcon(QIcon(":/mpimage/light/normal/play_all_normal.svg"));
    d->btPlayAll->setObjectName("MusicListDataPlayAll");
    d->btPlayAll->setText(tr("Play All"));
//    d->btPlayAll->setFixedWidth(93);
    d->btPlayAll->setFixedHeight(30);
    d->btPlayAll->setFocusPolicy(Qt::NoFocus);
    d->btPlayAll->setIconSize(QSize(18, 18));
    auto btPlayAllFont = d->btPlayAll->font();
    btPlayAllFont.setFamily("SourceHanSansSC");
    btPlayAllFont.setWeight(QFont::Medium);
//    btPlayAllFont.setPixelSize(13);
    DFontSizeManager::instance()->bind(d->btPlayAll, DFontSizeManager::T8);
    d->btPlayAll->setFont(btPlayAllFont);

    d->infoLabel = new DLabel;
    d->infoLabel->setObjectName("MusicListDataTitle");
    d->infoLabel->setText(tr("All Music"));
    d->infoLabel->setMinimumWidth(200);
    d->infoLabel->setWordWrap(true);
    d->infoLabel->setFont(btPlayAllFont);
    auto infoLabelPalette = d->infoLabel->palette();
    QColor infoLabelColor = infoLabelPalette.color(DPalette::BrightText);
    infoLabelColor.setAlphaF(0.7);
    infoLabelPalette.setColor(DPalette::ButtonText, infoLabelColor);
    d->infoLabel->setPalette(infoLabelPalette);
    d->infoLabel->setForegroundRole(DPalette::ButtonText);

//    d->btIconMode = new MusicImageButton(":/mpimage/light/normal/picture_list_normal.svg",
//                                         ":/mpimage/light/hover/picture_list_hover.svg",
//                                         ":/mpimage/light/press/picture_list_press.svg",
//                                         ":/mpimage/light/active/picture_list_active.svg");
    d->btIconMode = new DToolButton;
    d->btIconMode->setFixedSize(36, 36);
    d->btIconMode->setObjectName("MusicListDataWidgetIconMode");
    d->btIconMode->setCheckable(true);
    d->btIconMode->setChecked(false);

//    d->btlistMode = new MusicImageButton(":/mpimage/light/normal/text_list_normal.svg",
//                                         ":/mpimage/light/hover/text_list_hover.svg",
//                                         ":/mpimage/light/press/text_list_press.svg",
//                                         ":/mpimage/light/active/text_list_active.svg");
    d->btlistMode = new DToolButton;
    d->btlistMode->setFixedSize(36, 36);
    d->btlistMode->setObjectName("MusicListDataWidgetListMode");
    d->btlistMode->setCheckable(true);
    d->btlistMode->setChecked(false);

//    auto framelayout = new QHBoxLayout(this);
//    DFrame *t_frame = new DFrame(this);
//    framelayout->addWidget(d->btIconMode, 0, Qt::AlignCenter);
//    framelayout->addWidget(d->btlistMode, 0, Qt::AlignCenter);
//    t_frame->setLayout(framelayout);

    actionInfoBarLayout->addWidget(d->btPlayAll, 0, Qt::AlignVCenter);
    actionInfoBarLayout->addWidget(d->infoLabel, 100, Qt::AlignLeft | Qt::AlignVCenter);
    actionInfoBarLayout->addStretch();
    actionInfoBarLayout->addWidget(d->btIconMode, 0, Qt::AlignCenter);
    actionInfoBarLayout->addWidget(d->btlistMode, 0, Qt::AlignCenter);
    /*----------------fix bug----------------------*/
    actionBarLayout->addLayout(actionTileBarLayout);
    actionBarLayout->addLayout(actionInfoBarLayout);

    d->emptyHits = new DLabel();
    d->emptyHits->setObjectName("MusicListDataEmptyHits");
    d->emptyHits->hide();
    auto emptyHitsFont = d->emptyHits->font();
    emptyHitsFont.setFamily("SourceHanSansSC");
    emptyHitsFont.setPixelSize(20);
    d->emptyHits->setFont(emptyHitsFont);

    d->emptyHits->setText(MusicListDataWidget::tr("No songs"));
    d->emptyHits->setText(MusicListDataWidget::tr("No search results"));

    d->emptySearchHits = new DLabel();
    d->emptySearchHits->hide();
    auto emptySearchHitsFont = d->emptySearchHits->font();
    emptySearchHitsFont.setFamily("SourceHanSansSC");
    emptySearchHitsFont.setPixelSize(14);
    d->emptySearchHits->setFont(emptySearchHitsFont);

    auto emptyLayout = new QVBoxLayout();
    emptyLayout->setContentsMargins(0, 0, 0, 0);
    emptyLayout->setSpacing(10);

    emptyLayout->addStretch(100);
    emptyLayout->addWidget(d->emptyHits, 0, Qt::AlignCenter);
    emptyLayout->addWidget(d->emptySearchHits, 0, Qt::AlignCenter);
    emptyLayout->addStretch(100);

    d->albumListView = new MusicListDataView;
    d->artistListView = new MusicListDataView;
    d->musicListView = new PlayListView(true);
    d->musicListView->hide();

    layout->setContentsMargins(0, 1, 0, 0);

    /*---------------tabWidget----------------*/
    d->tabWidget = new DTabWidget();
    d->songListView     = new PlayListView(true);
    d->singerListView   = new MusicListDataView();
    d->albListView      = new MusicListDataView();
    d->tabWidget->addTab(d->songListView, QString(tr("Songs")));
    d->tabWidget->addTab(d->singerListView, QString(tr("Artists")));
    d->tabWidget->addTab(d->albListView, QString(tr("Albums")));
    d->tabWidget->setDocumentMode(true);
    d->tabWidget->hide();
    d->songListView->hide();
    d->singerListView->hide();
    d->albListView->hide();

    layout->addWidget(d->actionBar, 0, Qt::AlignTop);
    layout->addLayout(emptyLayout, 0);
    layout->addSpacing(8);
    layout->addWidget(d->albumListView, 100);
    layout->addWidget(d->artistListView, 100);
    layout->addWidget(d->musicListView, 100);
    layout->addWidget(d->tabWidget, 100);

    d->initConntion();
}

MusicListDataWidget::~MusicListDataWidget()
{
}

void MusicListDataWidget::setCustomSortType(PlaylistPtr playlist)
{
    Q_D(MusicListDataWidget);

    DDropdown *t_curDropdown = nullptr;
    if (playlist->id() == AlbumMusicListID) {
        t_curDropdown = d->albumDropdown;
    } else if (playlist->id() == ArtistMusicListID) {
        t_curDropdown = d->artistDropdown;
    } else {
        t_curDropdown = d->musicDropdown;
    }
    t_curDropdown->setCurrentAction(nullptr);
    t_curDropdown->setText(tr("Custom"));
}

void MusicListDataWidget::resultTabwidget(int index)
{
    Q_D(MusicListDataWidget);

    if (index == 0) {

        d->albumSearchDropdown->hide();
        d->artistSearchDropdown->hide();
        d->musicSearchDropdown->show();

        if (d->songListView->viewMode() == QListView::IconMode) {
            d->btIconMode->setChecked(true);
            d->btlistMode->setChecked(false);
        } else {
            d->btIconMode->setChecked(false);
            d->btlistMode->setChecked(true);
        }

        tabwidgetInfo(MusicPlaylists);

        d->singerListView->clearSelection();
        d->albListView->clearSelection();

    } else if (index == 1) {

        d->albumSearchDropdown->hide();
        d->artistSearchDropdown->show();
        d->musicSearchDropdown->hide();

        if (d->singerListView->viewMode() == QListView::IconMode) {
            d->btIconMode->setChecked(true);
            d->btlistMode->setChecked(false);
        } else {
            d->btIconMode->setChecked(false);
            d->btlistMode->setChecked(true);
        }

        tabwidgetInfo(ArtistPlaylists);

        d->songListView->clearSelection();
        d->albListView->clearSelection();

    } else if (index == 2) {

        d->albumSearchDropdown->show();
        d->artistSearchDropdown->hide();
        d->musicSearchDropdown->hide();

        if (d->albListView->viewMode() == QListView::IconMode) {
            d->btIconMode->setChecked(true);
            d->btlistMode->setChecked(false);
        } else {
            d->btIconMode->setChecked(false);
            d->btlistMode->setChecked(true);
        }

        tabwidgetInfo(AlbumPlaylists);

        d->songListView->clearSelection();
        d->singerListView->clearSelection();
    }
}

void MusicListDataWidget::tabwidgetInfo(PlaylistPtr infoPlaylist)
{
    Q_D(MusicListDataWidget);

    if (infoPlaylist == nullptr) {
        return ;
    }

    PlaylistPtr playlist = infoPlaylist;
    QString searchStr = playlist->searchStr();

    bool chineseFlag = false;
    for (auto ch : searchStr) {
        if (DMusic::PinyinSearch::isChinese(ch)) {
            chineseFlag = true;
            break;
        }
    }

    int allCount = 0;
    QFontMetrics titleFm(d->titleLabel->font());
    auto text = titleFm.elidedText(playlist->displayName(), Qt::ElideRight, 300);
    d->titleLabel->setText(tr("Search Results"));
    d->titleLabel->setToolTip(playlist->displayName());
    DDropdown *t_curDropdown = nullptr;
    if (playlist->id() == AlbumResultListID) {

        PlayMusicTypePtrList playMusicTypePtrList = playlist->playMusicTypePtrList();
        int musicCount = 0;
        int musicListCount = 0;
        for (auto action : playMusicTypePtrList) {
            if (searchStr.isEmpty()) {
                musicCount += action->playlistMeta.sortMetas.size();
                musicListCount++;
            } else {
                if (chineseFlag) {
                    if (action->name.contains(searchStr, Qt::CaseInsensitive)) {
                        musicCount += action->playlistMeta.sortMetas.size();
                        musicListCount++;
                    }
                } else {
                    if (searchStr.size() == 1) {
                        auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                        if (!curTextList.isEmpty() && curTextList.first().contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount += action->playlistMeta.sortMetas.size();
                            musicListCount++;
                        }
                    } else {
                        auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                        if (!curTextList.isEmpty() && curTextList.join("").contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount += action->playlistMeta.sortMetas.size();
                            musicListCount++;
                        }
                    }
                }
            }
        }
        QString infoStr;
        if (musicCount == 0) {
            infoStr = QString("   ") + MusicListDataWidget::tr("No songs");
        } else if (musicCount == 1) {
            infoStr = QString("   ") + MusicListDataWidget::tr("1 album - 1 song");
        } else {
            infoStr = QString("   ") + MusicListDataWidget::tr("%1 album - %2 songs").arg(musicListCount).arg(musicCount);
            if (musicListCount == 1) {
                infoStr = QString("   ") + MusicListDataWidget::tr("%1 album - %2 songs").arg(musicListCount).arg(musicCount);
            } else {
                infoStr = QString("   ") + MusicListDataWidget::tr("%1 albums - %2 songs").arg(musicListCount).arg(musicCount);
            }
        }
        allCount = musicListCount;
        d->infoLabel->setText(infoStr);
    } else if (playlist->id() == ArtistResultListID) {
        PlayMusicTypePtrList playMusicTypePtrList = playlist->playMusicTypePtrList();
        int musicCount = 0;
        int musicListCount = 0;
        for (auto action : playMusicTypePtrList) {
            if (searchStr.isEmpty()) {
                musicCount += action->playlistMeta.sortMetas.size();
                musicListCount++;
            } else {
                if (chineseFlag) {
                    if (action->name.contains(searchStr, Qt::CaseInsensitive)) {
                        musicCount += action->playlistMeta.sortMetas.size();
                        musicListCount++;
                    }
                } else {
                    if (searchStr.size() == 1) {
                        auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                        if (!curTextList.isEmpty() && curTextList.first().contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount += action->playlistMeta.sortMetas.size();
                            musicListCount++;
                        }
                    } else {
                        auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->name);
                        if (!curTextList.isEmpty() && curTextList.join("").contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount += action->playlistMeta.sortMetas.size();
                            musicListCount++;
                        }
                    }
                }
            }
        }
        QString infoStr;
        if (musicCount == 0) {
            infoStr = QString("   ") + MusicListDataWidget::tr("No songs");
        } else if (musicCount == 1) {
            infoStr = QString("   ") + MusicListDataWidget::tr("1 artist - 1 song");
        } else {
            if (musicListCount == 1) {
                infoStr = QString("   ") + MusicListDataWidget::tr("%1 artist - %2 songs").arg(musicListCount).arg(musicCount);
            } else {
                infoStr = QString("   ") + MusicListDataWidget::tr("%1 artists - %2 songs").arg(musicListCount).arg(musicCount);
            }
        }
        allCount = musicListCount;
        d->infoLabel->setText(infoStr);
    } else if (playlist->id() == MusicResultListID) {
        QString infoStr;
        int musicCount = 0;
        for (auto action : playlist->allmusic()) {
            if (searchStr.isEmpty()) {
                musicCount ++;
            } else {
                if (chineseFlag) {
                    if (action->title.contains(searchStr, Qt::CaseInsensitive)) {
                        musicCount++;
                    }
                } else {
                    if (searchStr.size() == 1) {
                        auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->title);
                        if (!curTextList.isEmpty() && curTextList.first().contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount++;
                        }
                    } else {
                        auto curTextList = DMusic::PinyinSearch::simpleChineseSplit(action->title);
                        if (!curTextList.isEmpty() && curTextList.join("").contains(searchStr, Qt::CaseInsensitive)) {
                            musicCount++;
                        }
                    }
                }
            }
        }

        if (musicCount == 0) {
            infoStr = QString("   ") + MusicListDataWidget::tr("No songs");
        } else if (musicCount == 1) {
            infoStr = QString("   ") + MusicListDataWidget::tr("1 song");
        } else {
            infoStr = QString("   ") + MusicListDataWidget::tr("%1 songs").arg(musicCount);
        }
        allCount = musicCount;
        d->infoLabel->setText(infoStr);

    }
}


PlaylistPtr MusicListDataWidget::curPlaylist()
{
    Q_D(MusicListDataWidget);
    return d->curPlaylist;
}

void MusicListDataWidget::onSearchText(QString str)
{
    Q_D(MusicListDataWidget);
    if (d->curPlaylist.isNull()) {
        qWarning() << "can not change to emptry playlist";
        return;
    }

    d->initData(d->curPlaylist, false, str);
}

void MusicListDataWidget::selectMusiclistChanged(PlaylistPtr playlist)
{
    if (playlist.isNull()) {
        qWarning() << "can not change to emptry playlist";
        return;
    }

    Q_D(MusicListDataWidget);

    playlist->setSearchStr("");

    d->initData(playlist, true, "");
}

void MusicListDataWidget::onMusiclistChanged(PlaylistPtr playlist)
{
    if (playlist.isNull()) {
        qWarning() << "can not change to emptry playlist";
        return;
    }

    Q_D(MusicListDataWidget);

    playlist->setSearchStr("");
    d->initData(playlist);
}

void MusicListDataWidget::onMusicListAdded(PlaylistPtr playlist, const MetaPtrList metalist)
{
    Q_D(MusicListDataWidget);
    auto curPlayList = d->curPlaylist;
    if (curPlayList == nullptr) {
        d->initData(playlist);
    } else {
        if (curPlayList->id() == AlbumMusicListID || curPlayList->id() == ArtistMusicListID) {
            d->initData(d->curPlaylist);
        } else if (curPlayList->id() == playlist->id()) {
            d->musicListView->onMusicListAdded(metalist);
            d->updateInfo();
        }
    }
}

void MusicListDataWidget::onMusicListRemoved(PlaylistPtr playlist, const MetaPtrList metalist)
{
    Q_D(MusicListDataWidget);

    if (playlist->id() ==  "musicResult") {

        d->songListView->onMusicListRemoved(metalist);

        QString infoStr;
        if (d->titleLabel->text() == MusicListDataWidget::tr("All Music")) {
            infoStr = QString("   ") + MusicListDataWidget::tr("%1 songs").arg(d->musicListView->rowCount());
            d->infoLabel->setText(infoStr);
            return;
        }
        infoStr = QString("   ") + MusicListDataWidget::tr("%1 songs").arg(d->songListView->rowCount());
        d->infoLabel->setText(infoStr);
        d->titleLabel->setText(MusicListDataWidget::tr("Search Results"));

    }

    if (playlist != d->curPlaylist) {
        if (playlist->id() == PlayMusicListID && playlist->allmusic().isEmpty()) {
            d->albumListView->setPlaying(nullptr);
            d->artistListView->setPlaying(nullptr);
            d->musicListView->setPlaying(nullptr);
        }
        return;
    }

    if (d->curPlaylist->id() == playlist->id()) {

        if (playlist->id() != AlbumMusicListID && playlist->id() != ArtistMusicListID && playlist->id() != AllMusicListID) {
            d->updateInfo();
            if (playlist->id() == d->musicListView->playlist()->id())
                d->musicListView->onMusicListRemoved(metalist);

        } else {

            d->updateInfo();
            d->albumListView->updateList();
            d->artistListView->updateList();
            if (playlist->id() == d->musicListView->playlist()->id()) {
                d->musicListView->onMusicListRemoved(metalist);
            }
        }
    }
}

void MusicListDataWidget::onMusiclistUpdate()
{
    Q_D(MusicListDataWidget);
    d->initData(d->curPlaylist);

    int t_count = 0;
    if (d->curPlaylist->id() == AlbumMusicListID ) {
        t_count = d->albumListView->playMusicTypePtrList().size();
    } else if (d->curPlaylist->id() == ArtistMusicListID) {
        t_count = d->artistListView->playMusicTypePtrList().size();
    } else {
        t_count = d->musicListView->playMetaPtrList().size();
    }

    if (t_count == 0 && !d->curPlaylist->searchStr().isEmpty()) {
        d->emptyHits->setText(tr("No result found"));
        d->albumListView->setVisible(false);
        d->artistListView->setVisible(false);
        d->musicListView->setVisible(false);
        d->emptyHits->setVisible(true);
    } else {
        d->emptyHits->setText("");
        d->emptyHits->hide();
    }
}

void MusicListDataWidget::onMusicPlayed(PlaylistPtr playlist, const MetaPtr Meta)
{
    Q_D(MusicListDataWidget);
    d->albumListView->setPlaying(Meta);
    d->artistListView->setPlaying(Meta);
    d->musicListView->setPlaying(Meta);
    d->albumListView->update();
    d->artistListView->update();
    d->musicListView->update();

    d->songListView->setPlaying(Meta);
    d->singerListView->setPlaying(Meta);
    d->albListView->setPlaying(Meta);
    d->songListView->update();
    d->singerListView->update();
    d->albListView->update();
}

void MusicListDataWidget::slotTheme(int type)
{
    Q_D(MusicListDataWidget);
    QString rStr;
    if (type == 1) {
        rStr = "light";
    } else {
        rStr = "dark";
    }
    if (type == 1) {
        auto palette = this->palette();
        QColor background("#FFFFFF");
        background.setAlphaF(0.1);
        palette.setColor(DPalette::Background, background);
        setPalette(palette);

        auto emptyHitsPalette = d->emptyHits->palette();
        QColor titleLabelPaletteColor("#000000");
        titleLabelPaletteColor.setAlphaF(0.3);
        emptyHitsPalette.setColor(QPalette::WindowText, titleLabelPaletteColor);
        d->emptyHits->setPalette(emptyHitsPalette);
        d->emptyHits->setForegroundRole(QPalette::WindowText);

        d->emptySearchHits->setPalette(emptyHitsPalette);
        d->emptySearchHits->setForegroundRole(QPalette::WindowText);

//        auto titleLabelPalette = d->infoLabel->palette();
//        titleLabelPalette.setColor(QPalette::WindowText, QColor("#000000"));
//        d->titleLabel->setPalette(titleLabelPalette);
//        d->titleLabel->setForegroundRole(QPalette::WindowText);

        auto playAllPalette = d->btPlayAll->palette();
        playAllPalette.setColor(DPalette::ButtonText, Qt::white);
        playAllPalette.setColor(DPalette::Light, QColor("#FD5E5E"));
        playAllPalette.setColor(DPalette::Dark, QColor("#ED5656"));
        d->btPlayAll->setPalette(playAllPalette);

        auto infoLabelPalette = d->infoLabel->palette();
        QColor infoLabelColor = infoLabelPalette.color(DPalette::BrightText);
        infoLabelColor.setAlphaF(0.7);
        infoLabelPalette.setColor(DPalette::ButtonText, infoLabelColor);
        d->infoLabel->setPalette(infoLabelPalette);

//        auto playAllPalette = d->btPlayAll->palette();
//        playAllPalette.setColor(DPalette::Dark, QColor("#FD5E5E"));
//        playAllPalette.setColor(DPalette::Light, QColor("#ED5656"));
//        d->btPlayAll->setPalette(palette);
    } else {
        auto palette = this->palette();
        QColor background("#252525");
        palette.setColor(DPalette::Background, background);
        setPalette(palette);

        auto emptyHitsPalette = d->emptyHits->palette();
        QColor titleLabelPaletteColor("#C0C6D4");
        titleLabelPaletteColor.setAlphaF(0.4);
        emptyHitsPalette.setColor(QPalette::WindowText, titleLabelPaletteColor);
        d->emptyHits->setPalette(emptyHitsPalette);
        d->emptyHits->setForegroundRole(QPalette::WindowText);

        d->emptySearchHits->setPalette(emptyHitsPalette);
        d->emptySearchHits->setForegroundRole(QPalette::WindowText);

//        auto titleLabelPalette = d->infoLabel->palette();
//        titleLabelPalette.setColor(DPalette::WindowText, QColor("#FFFFFF"));
//        d->titleLabel->setPalette(titleLabelPalette);
//        d->titleLabel->setForegroundRole(DPalette::WindowText);

        auto playAllPalette = d->btPlayAll->palette();
        playAllPalette.setColor(DPalette::ButtonText, "#FFFFFF");
        playAllPalette.setColor(DPalette::Light, QColor("#DA2D2D"));
        playAllPalette.setColor(DPalette::Dark, QColor("#A51B1B"));
        d->btPlayAll->setPalette(playAllPalette);

        auto infoLabelPalette = d->infoLabel->palette();
        QColor infoLabelColor = infoLabelPalette.color(DPalette::BrightText);
        infoLabelColor.setAlphaF(0.7);
        infoLabelPalette.setColor(DPalette::ButtonText, infoLabelColor);
        d->infoLabel->setPalette(infoLabelPalette);

//        auto playAllPalette = d->btPlayAll->palette();
//        playAllPalette.setColor(DPalette::Dark, QColor("#DA2D2D"));
//        playAllPalette.setColor(DPalette::Light, QColor("#A51B1B"));
//        d->btPlayAll->setPalette(palette);
    }

//    d->btIconMode->setPropertyPic(QString(":/mpimage/%1/normal/picture_list_normal.svg").arg(rStr),
//                                  QString(":/mpimage/%1/hover/picture_list_hover.svg").arg(rStr),
//                                  QString(":/mpimage/%1/press/picture_list_press.svg").arg(rStr),
//                                  QString(":/mpimage/%1/active/picture_list_active.svg").arg(rStr));
//    d->btlistMode->setPropertyPic(QString(":/mpimage/%1/normal/text_list_normal.svg").arg(rStr),
//                                  QString(":/mpimage/%1/hover/text_list_hover.svg").arg(rStr),
//                                  QString(":/mpimage/%1/press/text_list_press.svg").arg(rStr),
//                                  QString(":/mpimage/%1/active/text_list_active.svg").arg(rStr));

//    d->btIconMode->setIcon(QIcon(QString(":/mpimage/light/normal/picture_list_normal.svg")));
    d->btIconMode->setIcon(QIcon::fromTheme("picture_list_texts"));
    d->btIconMode->setIconSize(QSize(36, 36));
//    d->btlistMode->setIcon(QIcon(QString(":/mpimage/light/normal/text_list_normal.svg")));
    d->btlistMode->setIcon(QIcon::fromTheme("text_list_texts"));
    d->btlistMode->setIconSize(QSize(36, 36));
    d->albumListView->setThemeType(type);
    d->artistListView->setThemeType(type);
    d->musicListView->setThemeType(type);
    /*----------tabwidget-------------*/
    d->songListView->setThemeType(type);
    d->singerListView->setThemeType(type);
    d->albListView->setThemeType(type);
}

void MusicListDataWidget::changePicture(QPixmap pixmap, QPixmap sidebarPixmap, QPixmap albumPixmap)
{
    Q_D(MusicListDataWidget);
    if (d->albumListView->isVisible()) {
        d->albumListView->setPlayPixmap(pixmap, sidebarPixmap, albumPixmap);
    } else if (d->artistListView->isVisible()) {
        d->artistListView->setPlayPixmap(pixmap, sidebarPixmap, albumPixmap);
    } else if (d->albListView->isVisible()) {
        d->albListView->setPlayPixmap(pixmap, sidebarPixmap, albumPixmap);
    } else if (d->singerListView->isVisible()) {
        d->singerListView->setPlayPixmap(pixmap, sidebarPixmap, albumPixmap);
    } else if (d->songListView->isVisible()) {
        d->songListView->setPlayPixmap(pixmap, sidebarPixmap, albumPixmap);
    } else {
        d->musicListView->setPlayPixmap(pixmap, sidebarPixmap, albumPixmap);
    }
}

void MusicListDataWidget::retResult(QString searchText, QList<PlaylistPtr> resultlist)
{
    Q_D(MusicListDataWidget);

    PlaylistPtr retdata;
    QString search = "";
    int CurIndex = 0;
    bool flagMus = false;
    bool flagArt = false;
    bool flagAlb = false;

    if (searchText.isEmpty()) {

        d->initData(d->selectPlaylist, false, "");

    } else {

        for ( int i = 0; i < resultlist.size(); ++i) {
            if (resultlist.at(i)->id() == MusicResultListID) {
                retdata = resultlist.at(i);

                MusicPlaylists = resultlist.at(i);
                flagMus = true;
                CurIndex = 0;

                if (d->songListView->viewMode() != (resultlist.at(i)->viewMode())) {
                    d->songListView->setViewModeFlag((QListView::ViewMode)resultlist.at(i)->viewMode());
                }
                if (d->songListView->viewMode() == QListView::IconMode) {
                    d->btIconMode->setChecked(true);
                    d->btlistMode->setChecked(false);
                } else {
                    d->btIconMode->setChecked(false);
                    d->btlistMode->setChecked(true);
                }

                d->songListView->onMusiclistChanged(resultlist.at(i));

            } else if (resultlist.at(i)->id() == ArtistResultListID) {
                ArtistPlaylists = resultlist.at(i);
                retdata = resultlist.at(i);
                flagArt = true;
                CurIndex = 1;

                if (d->singerListView->viewMode() != (resultlist.at(i)->viewMode())) {
                    d->singerListView->setViewModeFlag((QListView::ViewMode)resultlist.at(i)->viewMode());
                }
                if (d->singerListView->viewMode() == QListView::IconMode) {
                    d->btIconMode->setChecked(true);
                    d->btlistMode->setChecked(false);
                } else {
                    d->btIconMode->setChecked(false);
                    d->btlistMode->setChecked(true);
                }
                d->singerListView->onMusiclistChanged(resultlist.at(i));

            } else if (resultlist.at(i)->id() == AlbumResultListID) {
                AlbumPlaylists = resultlist.at(i);
                retdata = resultlist.at(i);
                flagAlb = true;
                CurIndex = 2;

                if (d->albListView->viewMode() != (resultlist.at(i)->viewMode())) {
                    d->albListView->setViewModeFlag((QListView::ViewMode)resultlist.at(i)->viewMode());
                }
                if (d->albListView->viewMode() == QListView::IconMode) {
                    d->btIconMode->setChecked(true);
                    d->btlistMode->setChecked(false);
                } else {
                    d->btIconMode->setChecked(false);
                    d->btlistMode->setChecked(true);
                }
                d->albListView->onMusiclistChanged(resultlist.at(i));
            }
        }

        /*-------Circulation refresh--------*/
        for (int j = 0; j < 3; j++) {
            d->tabWidget->setCurrentIndex(j);
        }

        if (retdata->id() == MusicResultListID) {

            d->tabWidget->setCurrentIndex(0);

        } else if (retdata->id() == ArtistResultListID) {

            d->tabWidget->setCurrentIndex(1);

        } else if (retdata->id() == AlbumResultListID) {
            d->tabWidget->setCurrentIndex(2);
        }

        /*------Current display page------*/
        if (CurIndex == 0) {
            tabwidgetInfo(MusicPlaylists);
        } else if (CurIndex == 1) {
            tabwidgetInfo(ArtistPlaylists);

        } else if (CurIndex == 2) {
            tabwidgetInfo(AlbumPlaylists);
        }

        if ( flagMus & flagArt & flagAlb) {

            d->tabWidget->setCurrentIndex(0);
            CurIndex = 0;
        }

        /*---------Search without result------*/
        if (d->songListView->rowCount() == 0 && d->singerListView->rowCount() == 0 && d->albListView->rowCount() == 0 ) {
            d->initData(retdata, false, "noSearchResults");
            return;
        }
        d->updateFlag = true;

        if (CurIndex == 0) {
            d->initData(MusicPlaylists, false, search);
            tabwidgetInfo(MusicPlaylists);
            if (resultlist.size() == 1) {
                ArtistPlaylists = nullptr;
                d->singerListView->onMusiclistChanged(ArtistPlaylists);
                AlbumPlaylists = nullptr;
                d->albListView->onMusiclistChanged(AlbumPlaylists);
            }
            return;
        }
        if (CurIndex == 1) {
            d->initData(ArtistPlaylists, false, search);
            tabwidgetInfo(ArtistPlaylists);
            if (resultlist.size() == 1) {
                MusicPlaylists = nullptr;
                d->songListView->onMusiclistChanged(MusicPlaylists);
                AlbumPlaylists = nullptr;
                d->albListView->onMusiclistChanged(AlbumPlaylists);
            }
            return;
        }
        if (CurIndex == 2) {
            d->initData(AlbumPlaylists, false, search);
            tabwidgetInfo(AlbumPlaylists);
            if (resultlist.size() == 1) {
                MusicPlaylists = nullptr;
                d->songListView->onMusiclistChanged(MusicPlaylists);
                ArtistPlaylists = nullptr;
                d->singerListView->onMusiclistChanged(ArtistPlaylists);
            }
            return;
        }
    }
}

void MusicListDataWidget::CloseSearch()
{
    Q_D(MusicListDataWidget);

    d->updateFlag = false;
    d->initData(d->selectPlaylist, false, "");
}

void MusicListDataWidget::onCustomContextMenuRequest(const QPoint &pos, PlaylistPtr selectedlist, PlaylistPtr favlist, QList<PlaylistPtr> newlists, char type)
{
    Q_D(MusicListDataWidget);
    if (type == 2) {
        d->albumListView->showContextMenu(pos, d->musicListView->playlist(), favlist, newlists);
    } else if (type == 3) {
        d->artistListView->showContextMenu(pos, d->musicListView->playlist(), favlist, newlists);
    } else if (type == 4) {
        d->songListView->showContextMenu(pos, d->musicListView->playlist(), favlist, newlists);
    } else if (type == 5) {
        d->albListView->showContextMenu(pos, d->musicListView->playlist(), favlist, newlists);

    } else if (type == 6) {
        d->singerListView->showContextMenu(pos, d->musicListView->playlist(), favlist, newlists);

    } else {
        d->musicListView->showContextMenu(pos, d->musicListView->playlist(), favlist, newlists);
    }
}

void MusicListDataWidget::dragEnterEvent(QDragEnterEvent *event)
{
    DWidget::dragEnterEvent(event);
    if (event->mimeData()->hasFormat("text/uri-list")) {
        qDebug() << "acceptProposedAction" << event;
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        return;
    }
}

void MusicListDataWidget::dropEvent(QDropEvent *event)
{
    DWidget::dropEvent(event);
    Q_D(MusicListDataWidget);

    if (!event->mimeData()->hasFormat("text/uri-list")) {
        return;
    }

    auto urls = event->mimeData()->urls();
    QStringList localpaths;
    for (auto &url : urls) {
        localpaths << url.toLocalFile();
    }

    if (!localpaths.isEmpty() && !d->curPlaylist.isNull()) {
        Q_EMIT importSelectFiles(d->curPlaylist, localpaths);
    }
}

