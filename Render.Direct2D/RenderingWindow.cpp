#include "stdafx.h"
#include "RenderingWindow.h"
#include "Common.h"

LRESULT RenderingWindow::MessageHandler( UINT const message,
                                         WPARAM const wparam,
                                         LPARAM const lparam )
{
    switch( message )
    {
        case WM_CREATE:
            {
                CreateHandler();
                return 0;
            }

        case WM_PAINT:
            {
                PaintHandler();
                return 0;
            }

        case WM_DPICHANGED:
            {
                DpiHandler( wparam, lparam );
                return 0;
            }

        case WM_SIZE:
            {
                SizeHandler( wparam, lparam );
                return 0;
            }

        default:
            {
                return __super::MessageHandler
                (
                    message,
                    wparam,
                    lparam
                );
            }
    }
}

#pragma region Message Handlers

void RenderingWindow::AdjustDpiScaling()
{
    auto const monitor =
            MonitorFromWindow( m_window, MONITOR_DEFAULTTONEAREST );

    unsigned dpiX = 0;
    unsigned dpiY = 0;

    HR( GetDpiForMonitor( monitor,
                          MDT_EFFECTIVE_DPI,
                          &dpiX,
                          &dpiY
    ) );

    m_dpi.x = static_cast<float>( dpiX );
    m_dpi.y = static_cast<float>( dpiY );

    DBOUT( L"DPI -> X: " << m_dpi.x << " Y: " << m_dpi.y << endl );

    RECT size =
    {
        0,
        0,
        static_cast<unsigned>( LogicalToPhysical( m_WindowWidth, dpiX ) ),
        static_cast<unsigned>( LogicalToPhysical( m_WindowHeight, dpiY ) )
    };

    assert( AdjustWindowRect( &size, GetWindowLong( m_window, GWL_STYLE ), false ) );

    assert( SetWindowPos(
            m_window,
            nullptr,
            0, 0,
            size.right - size.left,
            size.bottom - size.top,
            SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER )
    );

    DBOUT( L"Adjusted: " << size.left << " " << size.top << " " << size.left - size.right << " " << size.bottom - size.top << endl );
}

void RenderingWindow::CreateHandler()
{
    AdjustDpiScaling();
    CreateFactoryAndGeometry();
}

void RenderingWindow::DpiHandler( LPARAM const wparam,
                                  WPARAM const lparam )
{
    try
    {
        m_dpi.x = LOWORD(wparam);
        m_dpi.y = HIWORD(wparam);

        auto const *suggested =
                reinterpret_cast<RECT const *>( lparam );

        assert( SetWindowPos(
            m_window,
            nullptr,
            suggested->left,
            suggested->top,
            suggested->right - suggested->left,
            suggested->bottom - suggested->top,
            SWP_NOACTIVATE | SWP_NOZORDER
        ) );
    }
    catch( ComException const &e )
    {
        DBOUT( L"DpiHandler failed 0x%X\n", e.result );

        assert( InvalidateRect( m_window, nullptr, false ) );
    }

    DBOUT( L"DPI Handler -> x: " << m_dpi.x << "L, Y: " << m_dpi.y << endl );
}

void RenderingWindow::SizeHandler( const LPARAM wparam, const WPARAM lparam )
{
    try
    {
        if( !IsDeviceCreated() ) return;

        if( SIZE_MINIMIZED == wparam ) return;

        unsigned const width = LOWORD(lparam);
        unsigned const height = HIWORD(lparam);

        m_size.width = width * 96 / m_dpi.x;
        m_size.height = height * 96 / m_dpi.y;

        HR( m_device->CreateSurface( width,
                                     height,
                                     DXGI_FORMAT_B8G8R8A8_UNORM,
                                     DXGI_ALPHA_MODE_PREMULTIPLIED,
                                     m_surface.ReleaseAndGetAddressOf() ) );

        HR( m_visual->SetContent( m_surface.Get() ) );
    }
    catch( const ComException &ex )
    {
        ReleaseDeviceResources();
    }
}

void RenderingWindow::PaintHandler()
{
    try
    {
        CreateDeviceResources();

        ReadyDevice();
    }
    catch( const ComException &ex )
    {
        ReleaseDeviceResources();
    }
}

#pragma endregion

#pragma region Device Management

bool RenderingWindow::IsDeviceCreated() const
{
    return m_device3d;
}

void RenderingWindow::CreateFactoryAndGeometry()
{
    D2D1_FACTORY_OPTIONS options = {};

    #ifdef _DEBUG

    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

    #endif

    HR( D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED,
                           options,
                           m_factory.GetAddressOf() ) );

    const auto ellipse = Ellipse( Point2F( 50.0f, 50.0f ), 49.0f, 49.0f );

    HR( m_factory->CreateEllipseGeometry( ellipse, m_geometry.GetAddressOf() ) );
}

void RenderingWindow::ReleaseDeviceResources()
{
    m_device3d.Reset();
}

void RenderingWindow::CreateDeviceResources()
{
    if( IsDeviceCreated() )
        return;

    CreateDevice3D();
    CreateDevice2D();
    CreateCompositionDevice();
}

void RenderingWindow::CreateDevice3D()
{
    assert( !IsDeviceCreated() );

    unsigned flags =
            D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;

    #ifndef _DEBUG
		flags = D3D11_CREATE_DEVICE_DEBUG;
    #endif

    HR( D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        nullptr, 0,
        D3D11_SDK_VERSION,
        m_device3d.GetAddressOf(),
        nullptr,
        nullptr
    ) );
}

void RenderingWindow::CreateDevice2D()
{
    assert( m_device3d != nullptr );

    HR( m_device3d.As( &m_dxgiDevice ) );

    D2D1_CREATION_PROPERTIES properties = {};

    #ifndef _DEBUG
        properties.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    #elseif

    #endif

    HR( m_factory->CreateDevice( m_dxgiDevice.Get(),
                                 m_device2d.GetAddressOf() ) );
}

void RenderingWindow::CreateCompositionDevice()
{
    HR( DCompositionCreateDevice2(
            m_device2d.Get(),
            __uuidof(m_device),
            reinterpret_cast<void **>( m_device.ReleaseAndGetAddressOf() ) )
    );

    HR( m_device->CreateTargetForHwnd(
            m_window,
            true,
            m_target.ReleaseAndGetAddressOf() )
    );

    HR( m_device->CreateVisual( m_visual.ReleaseAndGetAddressOf() ) );

    RECT rect = {};
    assert(GetClientRect(m_window, &rect));

    HR( m_device->CreateSurface( rect.right - rect.left,
                                 rect.bottom - rect.top,
                                 DXGI_FORMAT_B8G8R8A8_UNORM,
                                 DXGI_ALPHA_MODE_PREMULTIPLIED,
                                 m_surface.ReleaseAndGetAddressOf() ) );

    HR( m_visual->SetContent( m_surface.Get() ) );
    HR( m_target->SetRoot( m_visual.Get() ) );

    m_size.width = ( rect.right - rect.left ) * 96 / m_dpi.x;
    m_size.height = ( rect.bottom - rect.top ) * 96 / m_dpi.y;

    CreateDeviceSpecificResources();
}

void RenderingWindow::CreateDeviceSpecificResources()
{
    // Create Resources for the Rendering Loop
    ComPtr<ID2D1DeviceContext> dc;

    HR( m_device2d->CreateDeviceContext( D2D1_DEVICE_CONTEXT_OPTIONS_NONE, dc.GetAddressOf() ) );

    auto const color = static_cast<_D3DCOLORVALUE>( ColorF( 0.26f, 0.56f, 0.87f, 0.5f ) );

    ComPtr<ID2D1SolidColorBrush> solid_brush;

    HR( dc->CreateSolidColorBrush( color, solid_brush.ReleaseAndGetAddressOf() ) );

    m_brush = solid_brush;

    auto const monitor = MonitorFromWindow( m_window, MONITOR_DEFAULTTONEAREST );

    unsigned x = 0;
    unsigned y = 0;

    HR( GetDpiForMonitor( monitor,
                          MDT_EFFECTIVE_DPI,
                          &x,
                          &y ) );

    m_dpi.x = static_cast<float>( x );
    m_dpi.y = static_cast<float>( y );
}

void RenderingWindow::DestroyResources()
{
    m_device3d = nullptr;
    m_device2d = nullptr;
    m_dxgiDevice = nullptr;

    m_device = nullptr;
    m_target = nullptr;
    m_visual = nullptr;
    m_surface = nullptr;

    m_factory = nullptr;
    m_geometry = nullptr;
}

#pragma endregion

int RenderingWindow::MessagePump()
{
    MSG message = {};

    while( true )
    {
        if( PeekMessage( &message, m_window, 0, 0, PM_REMOVE ) )
        {
            if( WM_QUIT == message.message )
            {
                break;
            }

            TranslateMessage( &message );
            DispatchMessage( &message );
        }
        else
        {
            ReadyDevice();
        }
    }

    return message.wParam;
}

void RenderingWindow::ReadyDevice()
{
    ComPtr<ID2D1DeviceContext> dc;

    POINT offset = {};

    assert( m_surface );

    HR( m_surface->BeginDraw( nullptr, __uuidof(dc), reinterpret_cast<void **>( dc.GetAddressOf() ), &offset ) );

    dc->SetDpi( m_dpi.x, m_dpi.y );

    dc->SetTransform( Matrix3x2F::Translation( offset.x * 96 / m_dpi.x, offset.y * 96 / m_dpi.y ) );

    Render( dc.Get() );

    HR( m_surface->EndDraw() );

    HR( m_device->Commit() );
}

void RenderingWindow::Render( ID2D1DeviceContext *dc )
{
    for( auto x = 0; x < m_size.width; x += 10 )
    {
        dc->DrawLine
        (
            Point2F( static_cast<FLOAT>( x ), 0.0f ),
            Point2F( static_cast<FLOAT>( x ), m_size.height ),
            m_brush.Get(),
            0.5f
        );
    }

    for( auto y = 0; y < m_size.height; y += 10 )
    {
        dc->DrawLine
        (
            Point2F( 0.0f, static_cast<FLOAT>( y ) ),
            Point2F( static_cast<FLOAT>( m_size.width ), static_cast<FLOAT>( y ) ),
            m_brush.Get(),
            0.5f
        );
    }
}
