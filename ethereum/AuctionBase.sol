pragma solidity ^0.5.0;

import "./ERC721.sol";

contract AuctionBase {
    struct Auction {
        address seller;

        address winner;
        uint128 maxPrice;

        uint256 sumPrice;

        uint64 duration;

        uint64 startedAt;
    }

    ERC721 public nonFungibleContract;

    // TODO: setting minimum auctioneer and bid price
    uint16 constant minAuctioneer = 0;
    uint128 constant minPrice = 0;

    mapping (uint256 => Auction) tokenIdToAuction;

    mapping (uint256 => mapping (address => uint256)) tokenIdToBidderAndAmount;
    mapping (uint256 => address[]) tokenIdToBidders;

    event AuctionCreated(uint256 tokenId, uint256 duration);
    event AuctionSuccessful(uint256 tokenId, uint256 sumPrice, address winner);
    event AuctionCancelled(uint256 tokenId);

    event BidderCreated(uint256 tokenId, address bidder);
    event BidAmountUpdated(uint256 tokenId, address bidder, uint256 newAmount);

    function _owns(address _owner, uint256 tokenId) internal view returns (bool) {
        return nonFungibleContract.ownerOf(_tokenId) == _owner;
    }

    function _escrow(address _owner, uint256 tokenId) internal {
        nonFungibleContract.transferFrom(_owner, this, _tokenId);
    }

    function _transfer(address _to, uint256 _tokenId) internal {
        nonFungibleContract.transfer(_to, _tokenId);
    }

    function _addAuction(uint256 tokenId, Auction _auction) internal {
        // TODO: setting minimum duration
        require(_auction.duration >= 1 minutes);

        tokenIdToAuction[_tokenId] = _auction;

        emit AuctionCreated(uint256(_tokenId), uint256(_auction.duration));
    }

    function _removeAuction(uint256 _tokenId) internal {
        delete tokenIdToAuction[_tokenId];

        uint256 idx;
        for (idx = 0; idx < tokenIdToBidders[_tokenId].length; idx++) {
            delete tokenIdToBidderAndAmount[_tokenId][tokenIdToBidders[_tokenId][idx]];
        }

        delete tokenIdToBidderAndAmount[_tokenId];
        delete tokenIdToBidders[_tokenId];
    }

    function _cancelAuction(uint256 _tokenId, address _seller) internal {
        _removeAuction(_tokenId);
        _transfer(_seller, _tokenId);
        emit AuctionCancelled(_tokenId);
    }

    function _isOnAuction(Auction storage _auction) internal view returns (bool) {
        return _auction.startedAt > 0;
    }

    // @dev when bidder has not yet bid
    function _addBidder(uint256 _tokenId, address _bidder, uint256 _bidAmount) internal {
        require(tokenIdToBidderAndAmount[_tokenId][_bidder] == uint256(0));

        tokenIdToBiddersAndAmount[_tokenId][_bidder] = _bidAmount;
        tokenIdToBidders[_tokenId].push(_bidder);

        emit BidderCreated(_tokenId, _bidder.bidder);
    }

    // @dev when bidder has been bid
    function _updateBidAmount(uint256 _tokenId, address _bidder, uint256 _bidAmount) internal {
        Auction storage auction = tokenIdToAuction[_tokenId];
        uint256 currentPrice = 0;

        if (tokenIdToBidderAndAmount[_tokenId][_bidder] != uint256(0)) {
            currentPrice = tokenIdToBidderAndAmount[_tokenId][_bidder] + _bidAmount;

            tokenIdToBidderAndAmount[_tokenId][_bidder] = currentPrice;
            auction.sumPrice += _bidAmount;

            if (auction.maxPrice < currentPrice) {
                auction.winner = _bidder;
                auction.maxPrice = currentPrice;
            }
        }

        emit BidderUpdated(_tokenId, _bidder, _bidAmount);
    }

    function _isFinished(uint256 _tokenId) internal returns (bool) {
        uint256 passedSeconds = 0;

        Auction storage _auction = tokenIdToAuction[_tokenId];

        if (now > _auction.startedAt) {
            passedSeconds = now - _auction.startedAt;
        }

        if (passedSeconds > _auction.duration){
            return true;
        }

        return false;
    }

    // TODO: implement metric transfer ether to seller and owner
    function _bid(uint256 _tokenId, uint256 _bidAmount) internal {
        require(_isOnAuction(auction));
        require(!_isFinished(_tokenId));

        require(tokenIdToBidders[_tokenId].length >= minAuctioneer);
        require(_bidAmount >= minPrice);

        if (tokenIdToBidderAndAmount[_tokenId][msg.sender] == 0) {
            _addBidder(_tokenId, msg.sender, _bidAmount);
        } else {
            _updateBidAmount(_tokenId, msg.sender, _bidAmount);
        }

        // TODO: send ether to seller and owner
    }
}
