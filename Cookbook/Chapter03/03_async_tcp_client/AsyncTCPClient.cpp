#include "AsyncTCPClient.h"


AsyncTCPClient::AsyncTCPClient() 
{
    m_work_guard.reset( new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(m_io_ctx.get_executor() ) ) ;
    m_thread.reset(new std::thread([this](){ m_io_ctx.run(); } ) );
}


void AsyncTCPClient::emulateLongComputationOp( unsigned int duration_sec, const std::string& raw_ip_address, unsigned short port_num, Callback callback, unsigned int request_id) {

    std::string request = "EMULATE_LONG_CALC_OP " + std::to_string(duration_sec) + "\n";

    std::shared_ptr<Session> session = std::shared_ptr<Session>(new Session(m_io_ctx, raw_ip_address, port_num, request, request_id, callback));

    session->m_sock.open( session->m_ep.protocol() );

    std::unique_lock<std::mutex> lock(m_active_sessions_guard);

    m_active_sessions[request_id] = session;

    lock.unlock();

    session->m_sock.async_connect(session->m_ep, 

    [this, session](const boost::system::error_code& ec) 
    {
        if (ec.value() != 0) {

            session->m_ec = ec;
            onRequestComplete(session);
            return;
        }

        std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);

        if (session->m_was_cancelled) {

            onRequestComplete(session);
            return;
        }

        // std::this_thread::sleep_for(std::chrono::seconds(1)); // To check whether cancelling is possible
    

        boost::asio::async_write(session->m_sock, boost::asio::buffer(session->m_request),

        [this, session](const boost::system::error_code& ec, std::size_t bytes_transferred) 
        {
                if (ec.value() != 0) {

                    session->m_ec = ec;
                    onRequestComplete(session);
                    return;
                }

                std::unique_lock<std::mutex> cancel_lock(session->m_cancel_guard);

                if (session->m_was_cancelled) {

                    onRequestComplete(session);
                    return;
                }

                boost::asio::async_read_until(session->m_sock, session->m_response_buf, '\n', 

                [this, session](const boost::system::error_code& ec, std::size_t bytes_transferred) 
                {
                    if (ec.value() != 0) {
                        session->m_ec = ec;
                    } else {
                        std::istream strm( &session->m_response_buf );
                        std::getline(strm, session->m_response);
                    }
                    onRequestComplete(session);

                } /* end read callback */ );
            
        } /* end write callback */ ); 
        
    } /* end connect callback */ );

}

void AsyncTCPClient::cancelRequest(unsigned int request_id) {

    std::unique_lock<std::mutex> lock(m_active_sessions_guard);
    auto it = m_active_sessions.find(request_id);

    if (it != m_active_sessions.end()) {

        std::unique_lock<std::mutex> cancel_lock(it->second->m_cancel_guard);
        it->second->m_was_cancelled = true;
        it->second->m_sock.cancel();
    }
}


void AsyncTCPClient::close() {

    m_work_guard.reset(nullptr);
    m_thread->join();
}


void AsyncTCPClient::onRequestComplete(std::shared_ptr<Session> session) {

    boost::system::error_code ignored_ec;

    session->m_sock.shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

    std::unique_lock<std::mutex> lock(m_active_sessions_guard);

    auto it = m_active_sessions.find(session->m_id);

    if (it != m_active_sessions.end())

        m_active_sessions.erase(it);


    lock.unlock();


    boost::system::error_code ec;


    if (session->m_ec.value() == 0 && session->m_was_cancelled) {

        ec = boost::asio::error::operation_aborted;
    }
    else {
        ec = session->m_ec;
    }

    session->m_callback(session->m_id, session->m_response, ec);
};