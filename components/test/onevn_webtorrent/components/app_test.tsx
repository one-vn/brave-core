/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { shallow } from 'enzyme'
import { applicationState, torrentState, torrentObj } from '../testData'
import { TorrentState } from '../../../onevn_webtorrent/extension/constants/webtorrentState'
import { OneVNWebtorrentPage, mapStateToProps } from '../../../onevn_webtorrent/extension/components/app'

describe('OneVNWebtorrentPage component', () => {
  describe('mapStateToProps', () => {
    it('should map the default state', () => {
      expect(mapStateToProps(applicationState, { tabId: 0 })).toEqual({
        torrentState
      })
    })
  })

  describe('render OneVNWebtorrentPage component', () => {
    it('renders the MediaViewer component with a torrent and ix', () => {
      const torrentStateWithIx: TorrentState = { ...torrentState, ix: 1 }
      const wrapper = shallow(
        <OneVNWebtorrentPage
          torrentState={torrentStateWithIx}
          torrentObj={torrentObj}
          actions={{}}
        />
      ).dive()

      const assertion = wrapper.find('.mediaViewer')
      expect(assertion.length).toBe(1)
    })
    it('renders the TorrentViewer component with a valid torrent state', () => {
      const wrapper = shallow(
        <OneVNWebtorrentPage
          torrentState={torrentState}
          actions={{}}
        />
      ).dive()

      const assertion = wrapper.find('.torrent-viewer')
      expect(assertion.length).toBe(1)
    })
  })
})